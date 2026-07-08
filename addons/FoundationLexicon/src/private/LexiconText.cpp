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

#include "LexiconText.h"
#include "LexiconRegistry.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LexiconText::set_mode(int v) { mode = v; }
int LexiconText::get_mode() const { return mode; }

void LexiconText::set_key_or_value(const String &v) { key_or_value = v; }
String LexiconText::get_key_or_value() const { return key_or_value; }

bool LexiconText::is_localised() const { return mode == MODE_LOCALISED; }
bool LexiconText::is_literal() const { return mode == MODE_LITERAL; }
bool LexiconText::is_invariant() const { return mode == MODE_INVARIANT; }

uint64_t LexiconText::get_hash() const
{
    return key_or_value.is_empty() ? 0 : LexiconRegistry::get_singleton()->hash(key_or_value);
}

String LexiconText::resolve() const
{
    if (mode == MODE_LOCALISED)
    {
        const String resolved = LexiconRegistry::get_singleton()->resolve_string(get_hash());
        return resolved.is_empty() ? key_or_value : resolved;
    }
    return key_or_value;
}

void LexiconText::_bind_methods()
{
    BIND_ENUM_CONSTANT(MODE_LOCALISED);
    BIND_ENUM_CONSTANT(MODE_LITERAL);
    BIND_ENUM_CONSTANT(MODE_INVARIANT);

    ClassDB::bind_method(D_METHOD("set_mode", "mode"), &LexiconText::set_mode);
    ClassDB::bind_method(D_METHOD("get_mode"), &LexiconText::get_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Localised,Literal,Invariant"), "set_mode", "get_mode");

    ClassDB::bind_method(D_METHOD("set_key_or_value", "value"), &LexiconText::set_key_or_value);
    ClassDB::bind_method(D_METHOD("get_key_or_value"), &LexiconText::get_key_or_value);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "key_or_value"), "set_key_or_value", "get_key_or_value");

    ClassDB::bind_method(D_METHOD("is_localised"), &LexiconText::is_localised);
    ClassDB::bind_method(D_METHOD("is_literal"), &LexiconText::is_literal);
    ClassDB::bind_method(D_METHOD("is_invariant"), &LexiconText::is_invariant);
    ClassDB::bind_method(D_METHOD("get_hash"), &LexiconText::get_hash);
    ClassDB::bind_method(D_METHOD("resolve"), &LexiconText::resolve);
}
