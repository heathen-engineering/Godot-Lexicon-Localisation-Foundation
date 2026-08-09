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

@tool
extends Resource
class_name LexiconSourceList

## Authoring resource listing the project's .helex source files to register at boot.
## Each entry is a res:// path to a .helex (JSON) file. A source whose "assetId" is
## "default" (case-insensitive) becomes the culture-neutral last-resort fallback.
@export var helex_paths: PackedStringArray = PackedStringArray()

func register_all() -> void:
	var registry := Engine.get_singleton("LexiconRegistry")
	for path in helex_paths:
		if path.is_empty():
			continue
		var file := FileAccess.open(path, FileAccess.READ)
		if file == null:
			push_warning("LexiconSourceList: could not open .helex file: " + path)
			continue
		registry.register_source(file.get_as_text())
