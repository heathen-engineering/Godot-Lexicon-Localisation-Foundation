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

#include "LexiconSubsystem.h"

#include "LexiconRegistry.h"

std::string LexiconSubsystem::display_name() const
{
    return "Lexicon";
}

std::vector<std::pair<std::string, std::string>> LexiconSubsystem::debug_info() const
{
    std::vector<std::pair<std::string, std::string>> result;

    LexiconRegistry *registry = LexiconRegistry::get_singleton();
    if (registry == nullptr)
    {
        result.emplace_back("Registry", "not constructed");
        return result;
    }

    godot::String active = registry->get_active_culture();
    godot::String def = registry->get_default_culture();
    result.emplace_back("Active culture", active.is_empty() ? "(none)" : std::string(active.utf8().get_data()));
    result.emplace_back("Default culture", def.is_empty() ? "(none)" : std::string(def.utf8().get_data()));
    result.emplace_back("Registered sources", std::to_string(registry->get_registered_source_count()));
    return result;
}
