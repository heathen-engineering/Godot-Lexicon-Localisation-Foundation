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

#include "LexiconSound.h"
#include "LexiconRegistry.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LexiconSound::set_mode(int v) { mode = v; }
int LexiconSound::get_mode() const { return mode; }

void LexiconSound::set_key(const String &v) { key = v; }
String LexiconSound::get_key() const { return key; }

void LexiconSound::set_literal_sound(const Ref<AudioStream> &v) { literal_sound = v; }
Ref<AudioStream> LexiconSound::get_literal_sound() const { return literal_sound; }

bool LexiconSound::is_localised() const { return mode == MODE_LOCALISED; }
bool LexiconSound::is_literal() const { return mode == MODE_LITERAL; }
bool LexiconSound::is_invariant() const { return mode == MODE_INVARIANT; }

uint64_t LexiconSound::get_hash() const
{
    return key.is_empty() ? 0 : LexiconRegistry::get_singleton()->hash(key);
}

Ref<AudioStream> LexiconSound::resolve() const
{
    if (mode == MODE_LOCALISED)
    {
        const Ref<Resource> res = LexiconRegistry::get_singleton()->resolve_asset(get_hash());
        if (res.is_valid())
        {
            AudioStream *stream = Object::cast_to<AudioStream>(res.ptr());
            if (stream != nullptr)
                return Ref<AudioStream>(stream);
        }
        return literal_sound;
    }
    return literal_sound;
}

void LexiconSound::_bind_methods()
{
    BIND_ENUM_CONSTANT(MODE_LOCALISED);
    BIND_ENUM_CONSTANT(MODE_LITERAL);
    BIND_ENUM_CONSTANT(MODE_INVARIANT);

    ClassDB::bind_method(D_METHOD("set_mode", "mode"), &LexiconSound::set_mode);
    ClassDB::bind_method(D_METHOD("get_mode"), &LexiconSound::get_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Localised,Literal,Invariant"), "set_mode", "get_mode");

    ClassDB::bind_method(D_METHOD("set_key", "value"), &LexiconSound::set_key);
    ClassDB::bind_method(D_METHOD("get_key"), &LexiconSound::get_key);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "key"), "set_key", "get_key");

    ClassDB::bind_method(D_METHOD("set_literal_sound", "value"), &LexiconSound::set_literal_sound);
    ClassDB::bind_method(D_METHOD("get_literal_sound"), &LexiconSound::get_literal_sound);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "literal_sound", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"),
        "set_literal_sound", "get_literal_sound");

    ClassDB::bind_method(D_METHOD("is_localised"), &LexiconSound::is_localised);
    ClassDB::bind_method(D_METHOD("is_literal"), &LexiconSound::is_literal);
    ClassDB::bind_method(D_METHOD("is_invariant"), &LexiconSound::is_invariant);
    ClassDB::bind_method(D_METHOD("get_hash"), &LexiconSound::get_hash);
    ClassDB::bind_method(D_METHOD("resolve"), &LexiconSound::resolve);
}
