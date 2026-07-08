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

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

/// <summary>
/// A localisation-aware AudioStream field (voice lines, narrated dialogue, ...).
/// Same Localised/Literal/Invariant semantics as LexiconText, typed to AudioStream so the
/// Inspector filters the literal-value picker accordingly.
///
/// Ported from Unity-Lexicon-Localisation-Foundation's LexiconSound.
/// </summary>
class LexiconSound : public Resource
{
    GDCLASS(LexiconSound, Resource);

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
    Ref<AudioStream> literal_sound;

public:
    LexiconSound() = default;

    void set_mode(int v);
    int get_mode() const;

    void set_key(const String &v);
    String get_key() const;

    void set_literal_sound(const Ref<AudioStream> &v);
    Ref<AudioStream> get_literal_sound() const;

    bool is_localised() const;
    bool is_literal() const;
    bool is_invariant() const;

    uint64_t get_hash() const;

    /// Resolves the active-culture clip when localised (falling back to literal_sound
    /// when unregistered), or returns literal_sound directly in Literal/Invariant mode.
    Ref<AudioStream> resolve() const;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(LexiconSound::Mode);
