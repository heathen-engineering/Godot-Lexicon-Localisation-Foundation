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

#include "LexiconAsset.h"
#include "LexiconRegistry.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LexiconAsset::set_mode(int v) { mode = v; }
int LexiconAsset::get_mode() const { return mode; }

void LexiconAsset::set_key(const String &v) { key = v; }
String LexiconAsset::get_key() const { return key; }

void LexiconAsset::set_literal_asset(const Ref<Resource> &v) { literal_asset = v; }
Ref<Resource> LexiconAsset::get_literal_asset() const { return literal_asset; }

bool LexiconAsset::is_localised() const { return mode == MODE_LOCALISED; }
bool LexiconAsset::is_literal() const { return mode == MODE_LITERAL; }
bool LexiconAsset::is_invariant() const { return mode == MODE_INVARIANT; }

uint64_t LexiconAsset::get_hash() const
{
    return key.is_empty() ? 0 : LexiconRegistry::get_singleton()->hash(key);
}

Ref<Resource> LexiconAsset::resolve() const
{
    if (mode == MODE_LOCALISED)
    {
        const Ref<Resource> res = LexiconRegistry::get_singleton()->resolve_asset(get_hash());
        return res.is_valid() ? res : literal_asset;
    }
    return literal_asset;
}

void LexiconAsset::_bind_methods()
{
    BIND_ENUM_CONSTANT(MODE_LOCALISED);
    BIND_ENUM_CONSTANT(MODE_LITERAL);
    BIND_ENUM_CONSTANT(MODE_INVARIANT);

    ClassDB::bind_method(D_METHOD("set_mode", "mode"), &LexiconAsset::set_mode);
    ClassDB::bind_method(D_METHOD("get_mode"), &LexiconAsset::get_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Localised,Literal,Invariant"), "set_mode", "get_mode");

    ClassDB::bind_method(D_METHOD("set_key", "value"), &LexiconAsset::set_key);
    ClassDB::bind_method(D_METHOD("get_key"), &LexiconAsset::get_key);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "key"), "set_key", "get_key");

    ClassDB::bind_method(D_METHOD("set_literal_asset", "value"), &LexiconAsset::set_literal_asset);
    ClassDB::bind_method(D_METHOD("get_literal_asset"), &LexiconAsset::get_literal_asset);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "literal_asset", PROPERTY_HINT_RESOURCE_TYPE, "Resource"),
        "set_literal_asset", "get_literal_asset");

    ClassDB::bind_method(D_METHOD("is_localised"), &LexiconAsset::is_localised);
    ClassDB::bind_method(D_METHOD("is_literal"), &LexiconAsset::is_literal);
    ClassDB::bind_method(D_METHOD("is_invariant"), &LexiconAsset::is_invariant);
    ClassDB::bind_method(D_METHOD("get_hash"), &LexiconAsset::get_hash);
    ClassDB::bind_method(D_METHOD("resolve"), &LexiconAsset::resolve);
}
