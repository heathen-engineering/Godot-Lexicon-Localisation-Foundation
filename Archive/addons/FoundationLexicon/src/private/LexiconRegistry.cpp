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

#include "LexiconRegistry.h"

#include "xxhash.h"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <vector>

using namespace godot;

namespace
{
    LexiconHintType hint_from_string(const String &s)
    {
        const String lower = s.to_lower();
        if (lower == "string") return LexiconHintType::HINT_STRING;
        if (lower == "sound") return LexiconHintType::HINT_SOUND;
        if (lower == "texture") return LexiconHintType::HINT_TEXTURE;
        if (lower == "sprite") return LexiconHintType::HINT_SPRITE;
        if (lower == "packedscene" || lower == "prefab" || lower == "scene") return LexiconHintType::HINT_PACKED_SCENE;
        if (lower == "asset") return LexiconHintType::HINT_ASSET;
        return LexiconHintType::HINT_NONE;
    }

    LexiconHintType hint_from_resource(const Ref<Resource> &res)
    {
        if (res.is_null())
            return LexiconHintType::HINT_NONE;
        const String cls = res->get_class();
        if (cls == "AudioStream" || cls.begins_with("AudioStream"))
            return LexiconHintType::HINT_SOUND;
        if (cls == "Texture2D" || cls.begins_with("Texture") || cls == "CompressedTexture2D" || cls == "ImageTexture")
            return LexiconHintType::HINT_TEXTURE;
        if (cls == "AtlasTexture")
            return LexiconHintType::HINT_SPRITE;
        if (cls == "PackedScene")
            return LexiconHintType::HINT_PACKED_SCENE;
        return LexiconHintType::HINT_ASSET;
    }
} // namespace

LexiconRegistry *LexiconRegistry::singleton = nullptr;

LexiconRegistry::LexiconRegistry()
{
    singleton = this;
}

LexiconRegistry::~LexiconRegistry()
{
    if (singleton == this)
        singleton = nullptr;
}

LexiconRegistry *LexiconRegistry::get_singleton()
{
    return singleton;
}

LexiconRegistry::CultureKey LexiconRegistry::to_key(const String &s)
{
    return std::string(s.utf8().get_data());
}

// -----------------------------------------------------------------------
// Hashing
// -----------------------------------------------------------------------

uint64_t LexiconRegistry::hash(const String &text) const
{
    const CharString utf8 = text.utf8();
    return static_cast<uint64_t>(XXH3_64bits(utf8.ptr(), utf8.length()));
}

// -----------------------------------------------------------------------
// Validation
// -----------------------------------------------------------------------

bool LexiconRegistry::validate_key(const String &dot_path)
{
    if (dot_path.is_empty())
        return false;
    if (dot_path.begins_with(".") || dot_path.ends_with("."))
        return false;
    return true;
}

// -----------------------------------------------------------------------
// Source registration
// -----------------------------------------------------------------------

void LexiconRegistry::register_source(const String &helex_json)
{
    Ref<JSON> json;
    json.instantiate();
    if (json->parse(helex_json) != OK)
    {
        UtilityFunctions::push_warning("LexiconRegistry: failed to parse .helex JSON: ", json->get_error_message());
        return;
    }

    const Variant data = json->get_data();
    if (data.get_type() != Variant::DICTIONARY)
    {
        UtilityFunctions::push_warning("LexiconRegistry: .helex root is not a JSON object");
        return;
    }

    const Dictionary root = data;
    const String asset_id = root.get("assetId", String());
    const bool auto_register = root.get("registered", true);
    if (!auto_register)
        return;

    const bool is_default = asset_id.to_lower() == "default";

    std::vector<String> source_cultures;
    if (root.has("cultures"))
    {
        const Array culturesArr = root["cultures"];
        for (int64_t i = 0; i < culturesArr.size(); ++i)
            source_cultures.push_back(String(culturesArr[i]));
    }

    if (!root.has("entries"))
        return;
    const Dictionary entries = root["entries"];
    const Array keys = entries.keys();

    std::unordered_set<uint64_t> seen;
    std::vector<LexiconEntry> parsed;
    parsed.reserve(static_cast<size_t>(keys.size()));

    for (int64_t i = 0; i < keys.size(); ++i)
    {
        const String key = String(keys[i]).strip_edges();
        if (key.is_empty())
            continue;

        const uint64_t h = hash(key);
        if (!seen.insert(h).second)
            continue; // duplicate key within this source; keep the first

        const Variant value = entries[key];
        LexiconEntry entry;
        entry.key = key;

        if (value.get_type() == Variant::STRING)
        {
            entry.hint = LexiconHintType::HINT_STRING;
            entry.string_value = value;
        }
        else if (value.get_type() == Variant::DICTIONARY)
        {
            const Dictionary assetObj = value;
            entry.asset_path = assetObj.get("path", String());
            entry.asset_sub_name = assetObj.get("sub", String());
            entry.hint = hint_from_string(assetObj.get("hint", String()));
            if (entry.hint == LexiconHintType::HINT_NONE)
                entry.hint = LexiconHintType::HINT_ASSET;
        }
        else
        {
            continue; // unsupported entry shape
        }

        parsed.push_back(entry);
    }

    for (const String &culture : source_cultures)
    {
        if (culture.strip_edges().is_empty())
            continue;
        auto &dict = ensure_culture(culture);
        for (const LexiconEntry &e : parsed)
            dict[hash(e.key)] = e;
    }

    if (is_default)
        for (const LexiconEntry &e : parsed)
            default_index[hash(e.key)] = e;

    // Seed default/active culture from the first source that declares one.
    if (!source_cultures.empty())
    {
        if (default_culture.is_empty())
            default_culture = source_cultures[0];
        if (active_culture.is_empty())
            active_culture = source_cultures[0];
    }

    registered_source_count++;

    // Runtime-injected entries survive source registration and keep priority.
    for (auto &[culture, overrides] : runtime_overrides)
    {
        auto &target = ensure_culture(String(culture.c_str()));
        for (const auto &[k, e] : overrides)
            target[k] = e;
    }

    emit_signal("sources_changed");
}

void LexiconRegistry::unregister_all()
{
    cultures.clear();
    runtime_overrides.clear();
    default_index.clear();
    active_culture = String();
    default_culture = String();
    registered_source_count = 0;
    emit_signal("sources_changed");
}

int LexiconRegistry::get_registered_source_count() const
{
    return registered_source_count;
}

// -----------------------------------------------------------------------
// Culture
// -----------------------------------------------------------------------

void LexiconRegistry::use_culture(const String &culture_code)
{
    active_culture = culture_code;
    emit_signal("culture_changed", culture_code);
}

String LexiconRegistry::get_active_culture() const { return active_culture; }
String LexiconRegistry::get_default_culture() const { return default_culture; }

PackedStringArray LexiconRegistry::get_mapped_culture_codes() const
{
    PackedStringArray result;
    result.resize(static_cast<int64_t>(cultures.size()));
    int64_t i = 0;
    for (const auto &[culture, dict] : cultures)
        result[i++] = String(culture.c_str());
    return result;
}

String LexiconRegistry::get_display_name(const String &asset_id) const
{
    const String resolved = resolve_string_path("Language." + asset_id);
    return resolved.is_empty() ? asset_id : resolved;
}

// -----------------------------------------------------------------------
// Resolution
// -----------------------------------------------------------------------

std::unordered_map<uint64_t, LexiconEntry> &LexiconRegistry::ensure_culture(const String &culture)
{
    const CultureKey key = to_key(culture);
    auto it = cultures.find(key);
    if (it == cultures.end())
    {
        it = cultures.emplace(key, std::unordered_map<uint64_t, LexiconEntry>()).first;
        if (default_culture.is_empty())
            default_culture = culture;
        if (active_culture.is_empty())
            active_culture = culture;
    }
    return it->second;
}

std::unordered_map<uint64_t, LexiconEntry> &LexiconRegistry::ensure_override_culture(const String &culture)
{
    const CultureKey key = to_key(culture);
    auto it = runtime_overrides.find(key);
    if (it == runtime_overrides.end())
        it = runtime_overrides.emplace(key, std::unordered_map<uint64_t, LexiconEntry>()).first;
    return it->second;
}

String LexiconRegistry::resolve_write_culture(const String &culture_code) const
{
    if (!culture_code.is_empty())
        return culture_code;
    if (!active_culture.is_empty())
        return active_culture;
    if (!default_culture.is_empty())
        return default_culture;
    return "default";
}

bool LexiconRegistry::try_get_from_culture(const String &culture, uint64_t key, LexiconEntry &out) const
{
    if (culture.is_empty())
        return false;
    auto it = cultures.find(to_key(culture));
    if (it == cultures.end())
        return false;
    auto entryIt = it->second.find(key);
    if (entryIt == it->second.end())
        return false;
    out = entryIt->second;
    return true;
}

String LexiconRegistry::base_culture(const String &culture)
{
    if (culture.is_empty())
        return String();
    const int64_t dash = culture.find("-");
    return (dash > 0) ? culture.substr(0, dash) : String();
}

bool LexiconRegistry::try_get_entry(uint64_t key, LexiconEntry &out) const
{
    // 1. Exact active culture (e.g. "fr-CA")
    if (try_get_from_culture(active_culture, key, out))
        return true;
    // 2. Base language of active culture (e.g. "fr")
    if (try_get_from_culture(base_culture(active_culture), key, out))
        return true;
    // 3. Exact default culture (+ its base)
    if (!default_culture.is_empty() && default_culture != active_culture)
    {
        if (try_get_from_culture(default_culture, key, out))
            return true;
        if (try_get_from_culture(base_culture(default_culture), key, out))
            return true;
    }
    // 4. Last resort: the always-present "default" source.
    auto it = default_index.find(key);
    if (it != default_index.end())
    {
        out = it->second;
        return true;
    }
    return false;
}

String LexiconRegistry::resolve_string(uint64_t key) const
{
    LexiconEntry entry;
    if (try_get_entry(key, entry) && entry.hint == LexiconHintType::HINT_STRING)
        return entry.string_value;
    return String();
}

String LexiconRegistry::resolve_string_path(const String &dot_path) const
{
    return resolve_string(hash(dot_path));
}

Ref<Resource> LexiconRegistry::resolve_asset(uint64_t key) const
{
    LexiconEntry entry;
    if (!try_get_entry(key, entry))
        return Ref<Resource>();
    if (entry.asset.is_valid())
        return entry.asset;
    if (!entry.asset_path.is_empty())
    {
        const String load_path = entry.asset_sub_name.is_empty()
            ? entry.asset_path
            : entry.asset_path + "::" + entry.asset_sub_name;
        return ResourceLoader::get_singleton()->load(load_path);
    }
    return Ref<Resource>();
}

Ref<Resource> LexiconRegistry::resolve_asset_path(const String &dot_path) const
{
    return resolve_asset(hash(dot_path));
}

int LexiconRegistry::get_hint(uint64_t key) const
{
    LexiconEntry entry;
    if (try_get_entry(key, entry))
        return static_cast<int>(entry.hint);
    return static_cast<int>(LexiconHintType::HINT_NONE);
}

// -----------------------------------------------------------------------
// Runtime injection
// -----------------------------------------------------------------------

void LexiconRegistry::set_string(const String &dot_path, const String &value, const String &culture_code)
{
    if (dot_path.is_empty())
        return;
    const uint64_t key = hash(dot_path);
    const String culture = resolve_write_culture(culture_code);

    LexiconEntry entry;
    entry.key = dot_path;
    entry.hint = LexiconHintType::HINT_STRING;
    entry.string_value = value;

    ensure_culture(culture)[key] = entry;
    ensure_override_culture(culture)[key] = entry;
}

void LexiconRegistry::set_asset(const String &dot_path, const Ref<Resource> &asset, const String &culture_code)
{
    if (dot_path.is_empty())
        return;
    const uint64_t key = hash(dot_path);
    const String culture = resolve_write_culture(culture_code);

    LexiconEntry entry;
    entry.key = dot_path;
    entry.hint = hint_from_resource(asset);
    entry.asset = asset;

    ensure_culture(culture)[key] = entry;
    ensure_override_culture(culture)[key] = entry;
}

void LexiconRegistry::set_asset_path(const String &dot_path, const String &path, int hint, const String &sub_name, const String &culture_code)
{
    if (dot_path.is_empty() || path.is_empty())
        return;
    const uint64_t key = hash(dot_path);
    const String culture = resolve_write_culture(culture_code);

    LexiconEntry entry;
    entry.key = dot_path;
    entry.hint = static_cast<LexiconHintType>(hint);
    entry.asset_path = path;
    entry.asset_sub_name = sub_name;

    ensure_culture(culture)[key] = entry;
    ensure_override_culture(culture)[key] = entry;
}

void LexiconRegistry::remove_key(const String &dot_path, const String &culture_code)
{
    if (dot_path.is_empty())
        return;
    const uint64_t key = hash(dot_path);

    if (!culture_code.is_empty())
    {
        auto it = cultures.find(to_key(culture_code));
        if (it != cultures.end())
            it->second.erase(key);
        auto oit = runtime_overrides.find(to_key(culture_code));
        if (oit != runtime_overrides.end())
            oit->second.erase(key);
    }
    else
    {
        for (auto &[culture, dict] : cultures)
            dict.erase(key);
        for (auto &[culture, dict] : runtime_overrides)
            dict.erase(key);
    }
}

// -----------------------------------------------------------------------
// Bindings
// -----------------------------------------------------------------------

void LexiconRegistry::_bind_methods()
{
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_NONE);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_STRING);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_SOUND);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_TEXTURE);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_SPRITE);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_PACKED_SCENE);
    BIND_ENUM_CONSTANT(LexiconHintType::HINT_ASSET);

    ClassDB::bind_method(D_METHOD("hash", "text"), &LexiconRegistry::hash);
    ClassDB::bind_static_method("LexiconRegistry", D_METHOD("validate_key", "dot_path"), &LexiconRegistry::validate_key);

    ClassDB::bind_method(D_METHOD("register_source", "helex_json"), &LexiconRegistry::register_source);
    ClassDB::bind_method(D_METHOD("unregister_all"), &LexiconRegistry::unregister_all);
    ClassDB::bind_method(D_METHOD("get_registered_source_count"), &LexiconRegistry::get_registered_source_count);

    ClassDB::bind_method(D_METHOD("use_culture", "culture_code"), &LexiconRegistry::use_culture);
    ClassDB::bind_method(D_METHOD("get_active_culture"), &LexiconRegistry::get_active_culture);
    ClassDB::bind_method(D_METHOD("get_default_culture"), &LexiconRegistry::get_default_culture);
    ClassDB::bind_method(D_METHOD("get_mapped_culture_codes"), &LexiconRegistry::get_mapped_culture_codes);
    ClassDB::bind_method(D_METHOD("get_display_name", "asset_id"), &LexiconRegistry::get_display_name);

    ClassDB::bind_method(D_METHOD("resolve_string", "key"), &LexiconRegistry::resolve_string);
    ClassDB::bind_method(D_METHOD("resolve_string_path", "dot_path"), &LexiconRegistry::resolve_string_path);
    ClassDB::bind_method(D_METHOD("resolve_asset", "key"), &LexiconRegistry::resolve_asset);
    ClassDB::bind_method(D_METHOD("resolve_asset_path", "dot_path"), &LexiconRegistry::resolve_asset_path);
    ClassDB::bind_method(D_METHOD("get_hint", "key"), &LexiconRegistry::get_hint);

    ClassDB::bind_method(D_METHOD("set_string", "dot_path", "value", "culture_code"), &LexiconRegistry::set_string, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("set_asset", "dot_path", "asset", "culture_code"), &LexiconRegistry::set_asset, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("set_asset_path", "dot_path", "path", "hint", "sub_name", "culture_code"),
        &LexiconRegistry::set_asset_path, DEFVAL(String()), DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("remove_key", "dot_path", "culture_code"), &LexiconRegistry::remove_key, DEFVAL(String()));

    ADD_SIGNAL(MethodInfo("culture_changed", PropertyInfo(Variant::STRING, "culture_code")));
    ADD_SIGNAL(MethodInfo("sources_changed"));
}
