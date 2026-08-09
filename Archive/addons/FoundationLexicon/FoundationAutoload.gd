# Copyright (c) 2026 Heathen Engineering Limited
# Irish Registered Company #556277
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

extends Node

## Add as an AutoLoad. Registers every configured LexiconSourceList's .helex sources
## into the LexiconRegistry singleton on _ready(), then activates the OS locale
## (falling back to the registry's detected default culture if unset).
@export var source_lists: Array[LexiconSourceList] = []

func _ready() -> void:
	var registry := Engine.get_singleton("LexiconRegistry")
	for list in source_lists:
		if list != null:
			list.register_all()

	var system_locale := OS.get_locale()
	if not system_locale.is_empty():
		registry.use_culture(system_locale)
