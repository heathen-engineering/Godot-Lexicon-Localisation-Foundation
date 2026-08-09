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
/// A localisation-aware string field for use on Resources/nodes (e.g. an Ogham dialogue
/// line). Authored as a Resource so it edits cleanly in the Inspector and can live inline
/// on other Resources (graph nodes, item definitions, ...).
///
/// Localised — key_or_value holds a dot-path key (e.g. "UI.Play"); resolve() looks it up
///             in LexiconRegistry for the active culture, falling back to key_or_value
///             itself if no entry is found.
/// Literal   — key_or_value holds the raw string directly; resolve() returns it unchanged.
/// Invariant — same storage/behaviour as Literal; the distinct mode exists so a future
///             gathering/export tool can skip these fields deliberately.
///
/// Ported from Unity-Lexicon-Localisation-Foundation's LexiconText.
/// </summary>
class LexiconText : public Resource
{
    GDCLASS(LexiconText, Resource);

public:
    enum Mode
    {
        MODE_LOCALISED = 0,
        MODE_LITERAL = 1,
        MODE_INVARIANT = 2,
    };

private:
    int mode = MODE_LITERAL;
    String key_or_value;

public:
    LexiconText() = default;

    void set_mode(int v);
    int get_mode() const;

    void set_key_or_value(const String &v);
    String get_key_or_value() const;

    bool is_localised() const;
    bool is_literal() const;
    bool is_invariant() const;

    uint64_t get_hash() const;

    /// Resolves the active-culture string when localised (falling back to key_or_value
    /// when unregistered), or returns key_or_value directly in Literal/Invariant mode.
    String resolve() const;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(LexiconText::Mode);
