@tool
extends RefCounted
class_name LexiconVocabulary

## Aggregates every dot-path key across every ".helex" file in the project, for the
## Lexicon key-picker EditorProperty and the key-browser dock. ".helex" files are the
## authoritative source at edit-time (mirrors GameplayTagVocabulary's reasoning for
## ".gptags" — autoloads that register sources into LexiconRegistry don't run while
## just editing in the Godot editor).

## One row per key: which cultures (besides "default") actually have an entry.
class KeyInfo:
	var key: String
	var cultures: PackedStringArray = PackedStringArray()

## Returns every ".helex" file under res://, recursively.
static func find_helex_files(root: String = "res://") -> PackedStringArray:
	var result := PackedStringArray()
	_scan_dir(root, result)
	return result

static func _scan_dir(path: String, out_files: PackedStringArray) -> void:
	var dir := DirAccess.open(path)
	if dir == null:
		return
	dir.list_dir_begin()
	var entry := dir.get_next()
	while entry != "":
		if entry == "." or entry == "..":
			entry = dir.get_next()
			continue
		var full_path := path.path_join(entry)
		if dir.current_is_dir():
			if entry != ".godot":
				_scan_dir(full_path, out_files)
		elif entry.ends_with(".helex"):
			out_files.append(full_path)
		entry = dir.get_next()
	dir.list_dir_end()

## Parses every ".helex" file and returns key -> KeyInfo (cultures that have an entry
## for that key; a source with assetId "default" contributes under the pseudo-culture
## "default" — the last-resort fallback tier, not a real BCP-47 culture).
static func get_all_keys() -> Dictionary:
	var keys := {}
	for path in find_helex_files():
		var file := FileAccess.open(path, FileAccess.READ)
		if file == null:
			continue
		var parsed = JSON.parse_string(file.get_as_text())
		if typeof(parsed) != TYPE_DICTIONARY:
			continue
		if not parsed.get("registered", true):
			continue
		var asset_id: String = parsed.get("assetId", "")
		var cultures: Array = parsed.get("cultures", [])
		var culture_tags: PackedStringArray = PackedStringArray(cultures)
		if asset_id.to_lower() == "default":
			culture_tags.append("default")
		var entries: Dictionary = parsed.get("entries", {})
		for key in entries.keys():
			if not keys.has(key):
				var info := KeyInfo.new()
				info.key = key
				keys[key] = info
			var info: KeyInfo = keys[key]
			for c in culture_tags:
				if not info.cultures.has(c):
					info.cultures.append(c)
	return keys

## Flat sorted list of every known key (for the picker's Tree).
static func get_all_key_names() -> PackedStringArray:
	var names := PackedStringArray(get_all_keys().keys())
	names.sort()
	return names

## Nested Dictionary tree from dot-path keys, mirroring GameplayTagVocabulary.build_tree.
static func build_tree(keys: PackedStringArray) -> Dictionary:
	var root := {}
	for key in keys:
		var node := root
		for segment in key.split("."):
			if not node.has(segment):
				node[segment] = {}
			node = node[segment]
	return root
