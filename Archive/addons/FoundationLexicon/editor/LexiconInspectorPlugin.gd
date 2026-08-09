@tool
extends EditorInspectorPlugin
class_name LexiconInspectorPlugin

## Registers the compact LexiconText/LexiconAsset/LexiconSound editors, and swaps the
## key-picker in for any other object's "lexicon_key"-suffixed String field (naming
## convention, matching FoundationGameplayTags' "_tag_name" convention — no
## compile-time dependency between addons).
##
## Deliberately uses object.is_class("LexiconText"/etc.) instead of `object is LexiconText`
## — those are native gdextension classes that stay inert until Extension Resolver unlocks
## the gate. `is Type` is a static identifier GDScript must resolve at PARSE time, and this
## script's own class_name makes it a target of Godot's eager project-wide class scan on
## editor startup, independent of when anyone actually load()s it — same bug class found and
## fixed in FoundationGameplayTags' GameplayTagsInspectorPlugin.gd this session. is_class()
## is a plain method call with a String literal, so the parser never needs the type to exist.

const TEXT_PROPERTIES := ["mode", "key_or_value"]
const ASSET_PROPERTIES := ["mode", "key", "literal_asset"]
const SOUND_PROPERTIES := ["mode", "key", "literal_sound"]

func _can_handle(object: Object) -> bool:
	return true

func _parse_begin(object: Object) -> void:
	if object.is_class("LexiconText"):
		add_property_editor_for_multiple_properties("Text", TEXT_PROPERTIES, LexiconTextCompactEditor.new())
	elif object.is_class("LexiconAsset"):
		add_property_editor_for_multiple_properties("Asset", ASSET_PROPERTIES, LexiconAssetCompactEditor.new())
	elif object.is_class("LexiconSound"):
		add_property_editor_for_multiple_properties("Sound", SOUND_PROPERTIES, LexiconSoundCompactEditor.new())

func _parse_property(object: Object, type: int, name: String, hint_type: int, hint_string: String, usage_flags: int, wide: bool) -> bool:
	if object.is_class("LexiconText") and TEXT_PROPERTIES.has(name):
		return true
	if object.is_class("LexiconAsset") and ASSET_PROPERTIES.has(name):
		return true
	if object.is_class("LexiconSound") and SOUND_PROPERTIES.has(name):
		return true

	if type == TYPE_STRING and name == "lexicon_key":
		add_property_editor(name, LexiconKeyPickerProperty.new())
		return true

	return false
