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

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

/// <summary>
/// A localisation-aware generic asset reference (textures, packed scenes, or any other
/// Resource) — the catch-all counterpart to LexiconText/LexiconSound. Same
/// Localised/Literal/Invariant semantics; consolidates what Unity splits into
/// LexiconAsset/LexiconTexture/LexiconSprite/LexiconPrefab, since Godot's exported
/// Resource-typed properties don't need a distinct C++ class per target type the way
/// Unity's serialised fields do.
///
/// Ported from Unity-Lexicon-Localisation-Foundation's LexiconAsset.
/// </summary>
class LexiconAsset : public Resource
{
    GDCLASS(LexiconAsset, Resource);

public:
    enum Mode
    {
        MODE_LOCALISED = 0,
        MODE_LITERAL = 1,
        MODE_INVARIANT = 2,
    };

private:
    int mode = MODE_LITERAL;
    String key;
    Ref<Resource> literal_asset;

public:
    LexiconAsset() = default;

    void set_mode(int v);
    int get_mode() const;

    void set_key(const String &v);
    String get_key() const;

    void set_literal_asset(const Ref<Resource> &v);
    Ref<Resource> get_literal_asset() const;

    bool is_localised() const;
    bool is_literal() const;
    bool is_invariant() const;

    uint64_t get_hash() const;

    /// Resolves the active-culture asset when localised (falling back to literal_asset
    /// when unregistered), or returns literal_asset directly in Literal/Invariant mode.
    Ref<Resource> resolve() const;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(LexiconAsset::Mode);
