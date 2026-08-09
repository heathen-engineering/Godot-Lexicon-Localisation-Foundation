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

#include "register_types.h"
#include "LexiconAsset.h"
#include "LexiconRegistry.h"
#include "LexiconSound.h"
#include "LexiconSubsystem.h"
#include "LexiconText.h"

#include <gameframework/SubsystemManager.h>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

static LexiconRegistry *lexicon_singleton = nullptr;

void initialize_foundation_lexicon_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    ClassDB::register_class<LexiconRegistry>();
    ClassDB::register_class<LexiconText>();
    ClassDB::register_class<LexiconSound>();
    ClassDB::register_class<LexiconAsset>();

    lexicon_singleton = memnew(LexiconRegistry);
    Engine::get_singleton()->register_singleton("LexiconRegistry", LexiconRegistry::get_singleton());

    // Real gameframework::Subsystem registration — see Godot-Game-Framework's
    // README, "The linking model", and FoundationGameplayTags' register_types.cpp
    // for the reference implementation this mirrors.
    gameframework::SubsystemManager::instance().register_subsystem<LexiconSubsystem>();
}

void uninitialize_foundation_lexicon_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    if (lexicon_singleton == nullptr)
        return;

    Engine::get_singleton()->unregister_singleton("LexiconRegistry");
    memdelete(lexicon_singleton);
    lexicon_singleton = nullptr;
}

extern "C"
{
    GDE_EXPORT GDExtensionBool foundation_lexicon_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization)
    {
        GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_foundation_lexicon_module);
        init_obj.register_terminator(uninitialize_foundation_lexicon_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
