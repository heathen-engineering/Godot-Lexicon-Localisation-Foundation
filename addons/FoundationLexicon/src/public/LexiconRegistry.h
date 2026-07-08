/*
 * Copyright (c) 2026 Heathen Engineering Limited
 * Irish Registered Company #556277
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace godot;

/// Content type of a resolved Lexicon entry. Stored per-entry so callers (and the future
/// key-picker editor tooling) can classify a key without loading the underlying resource.
enum LexiconHintType : uint8_t
{
    HINT_NONE = 0,
    HINT_STRING = 1,
    HINT_SOUND = 2,
    HINT_TEXTURE = 3,
    HINT_SPRITE = 4,
    HINT_PACKED_SCENE = 5,
    HINT_ASSET = 6, // catch-all
};

/// A single resolved Lexicon entry held in the registry's per-culture tables.
struct LexiconEntry
{
    String key;
    LexiconHintType hint = LexiconHintType::HINT_NONE;

    // Used when hint == HINT_STRING.
    String string_value;

    // Used when hint is any asset type. A direct reference (injected via set_asset(), or
    // resolved once and cached) wins over asset_path. asset_path is a res:// path — Godot's
    // native addressable identifier, standing in for Unity's Addressables GUID / O3DE's
    // compiled AZ::Uuid; ResourceLoader::load() resolves it on demand.
    Ref<Resource> asset;
    String asset_path;
    String asset_sub_name;
};

/// <summary>
/// Engine singleton ("LexiconRegistry") — the central runtime store for all Lexicon
/// localisation data. Maintains a per-culture table of entries parsed from .helex sources,
/// plus a runtime-override table (set_string/set_asset) that survives source registration,
/// and resolves strings/assets for the active culture through a fallback chain:
/// exact culture -> base language of active culture -> default culture (+ its base)
/// -> the culture-neutral "default" source.
///
/// Ported from Unity-GameplayTags-Foundation's sibling Unity-Lexicon-Localisation-Foundation
/// (treated as the gold-standard reference over the earlier O3DE-Lexicon-Foundation port).
/// Asset addressing is adapted to Godot's native res:// paths in place of Unity's
/// Addressables GUID streaming seam / O3DE's compiled AZ::Uuid blob.
/// </summary>
class LexiconRegistry : public Object
{
    GDCLASS(LexiconRegistry, Object);

private:
    static LexiconRegistry *singleton;

    using CultureKey = std::string;
    static CultureKey to_key(const String &s);

    // culture -> (key hash -> entry)
    std::unordered_map<CultureKey, std::unordered_map<uint64_t, LexiconEntry>> cultures;

    // Runtime-injected entries (set_string/set_asset/set_asset_path). Re-applied on top of
    // freshly registered sources so they survive and keep priority.
    std::unordered_map<CultureKey, std::unordered_map<uint64_t, LexiconEntry>> runtime_overrides;

    // Flattened hash -> entry index of the culture-neutral "default" source, for O(1)
    // last-resort lookup regardless of active culture.
    std::unordered_map<uint64_t, LexiconEntry> default_index;

    String active_culture;
    String default_culture;
    int registered_source_count = 0;

    bool try_get_entry(uint64_t key, LexiconEntry &out) const;
    bool try_get_from_culture(const String &culture, uint64_t key, LexiconEntry &out) const;
    static String base_culture(const String &culture);

    std::unordered_map<uint64_t, LexiconEntry> &ensure_culture(const String &culture);
    std::unordered_map<uint64_t, LexiconEntry> &ensure_override_culture(const String &culture);
    String resolve_write_culture(const String &culture_code) const;

public:
    LexiconRegistry();
    ~LexiconRegistry() override;

    static LexiconRegistry *get_singleton();

    // -----------------------------------------------------------------------
    // Hashing
    // -----------------------------------------------------------------------

    uint64_t hash(const String &text) const;

    // -----------------------------------------------------------------------
    // Source registration (.helex JSON)
    // -----------------------------------------------------------------------

    /// Parses .helex JSON text and registers its entries. Schema:
    /// { "assetId": "...", "registered": true, "cultures": ["en-GB"],
    ///   "entries": { "UI.Play": "Play", "UI.Logo": { "path": "res://...", "sub": "", "hint": "Sprite" } } }
    /// An assetId of "default" (case-insensitive) registers the entries as the culture-neutral
    /// last-resort fallback tier in addition to their declared cultures. Never throws: malformed
    /// JSON logs a warning and is a no-op. Emits "sources_changed" on success.
    void register_source(const String &helex_json);

    /// Clears every registered source, runtime override, and culture — full reset.
    void unregister_all();

    int get_registered_source_count() const;

    // -----------------------------------------------------------------------
    // Culture
    // -----------------------------------------------------------------------

    /// Sets the active culture and emits "culture_changed". Primary API for switching
    /// language at runtime (e.g. from a settings menu).
    void use_culture(const String &culture_code);

    String get_active_culture() const;
    String get_default_culture() const;
    PackedStringArray get_mapped_culture_codes() const;

    /// Localised display name for a culture, resolved as "Language.{cultureOrAssetId}",
    /// falling back to the input string itself when no entry is found.
    String get_display_name(const String &asset_id) const;

    // -----------------------------------------------------------------------
    // Resolution
    // -----------------------------------------------------------------------

    String resolve_string(uint64_t key) const;
    String resolve_string_path(const String &dot_path) const;

    /// Resolves an asset entry: a direct injected reference wins; otherwise loads asset_path
    /// via ResourceLoader on demand.
    Ref<Resource> resolve_asset(uint64_t key) const;
    Ref<Resource> resolve_asset_path(const String &dot_path) const;

    int get_hint(uint64_t key) const; // LexiconHintType, or HINT_NONE if not found

    // -----------------------------------------------------------------------
    // Runtime injection
    // -----------------------------------------------------------------------

    /// Injects or overwrites a string entry. Uses the active culture when culture_code is empty.
    void set_string(const String &dot_path, const String &value, const String &culture_code = String());

    /// Injects or overwrites a direct asset reference entry. Hint is inferred from the
    /// resource's class. Uses the active culture when culture_code is empty.
    void set_asset(const String &dot_path, const Ref<Resource> &asset, const String &culture_code = String());

    /// Injects or overwrites a path-addressed (lazily loaded) asset entry.
    void set_asset_path(const String &dot_path, const String &path, int hint, const String &sub_name = String(), const String &culture_code = String());

    /// Removes a runtime-injected entry. Pass an empty culture_code to remove from every culture.
    void remove_key(const String &dot_path, const String &culture_code = String());

    // -----------------------------------------------------------------------
    // Validation
    // -----------------------------------------------------------------------

    static bool validate_key(const String &dot_path);

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(LexiconHintType);
