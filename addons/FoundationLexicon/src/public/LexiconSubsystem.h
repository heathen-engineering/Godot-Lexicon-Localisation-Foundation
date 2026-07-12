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

#include <gameframework/Subsystem.h>

/// <summary>
/// Global framework Subsystem exposing LexiconRegistry's state to Godot-Game-
/// Framework's Subsystems dock and to other gems' depends_on() ordering.
/// Mirrors GameplayTagsSubsystem's shape and reasoning (see that class's own
/// header comment for why this is a read-only reporting surface, not a
/// lifecycle owner — Unity's LexiconSubsystem resets the registry for a
/// fresh session because Unity domain-reload reuses static state across
/// play sessions; Godot's module init runs fresh once per process, and
/// SubsystemManager::boot() is user/dock-triggered rather than automatic,
/// so a forced reset here could clobber a project's own already-completed
/// startup registration).
/// </summary>
class LexiconSubsystem : public gameframework::Subsystem
{
public:
    std::string display_name() const override;
    std::vector<std::pair<std::string, std::string>> debug_info() const override;
};
