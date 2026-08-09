@tool
extends EditorProperty
class_name LexiconSoundCompactEditor

## One-row compact editor for LexiconSound: Mode | (key picker when Localised, or an
## EditorResourcePicker filtered to AudioStream when Literal/Invariant).

const MODES := ["Localised", "Literal", "Invariant"]

var _mode: OptionButton
var _key_picker: LexiconKeyPickerProperty
var _resource_picker: EditorResourcePicker
var _updating := false

func _init() -> void:
	var row := HBoxContainer.new()

	_mode = OptionButton.new()
	for m in MODES:
		_mode.add_item(m)
	_mode.item_selected.connect(_on_mode_selected)
	row.add_child(_mode)

	_key_picker = LexiconKeyPickerProperty.new()
	_key_picker.custom_minimum_size = Vector2(200, 0)
	_key_picker.property_changed.connect(_relay_property_changed)
	row.add_child(_key_picker)

	_resource_picker = EditorResourcePicker.new()
	_resource_picker.base_type = "AudioStream"
	_resource_picker.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_resource_picker.connect("resource_changed", _on_resource_changed)
	row.add_child(_resource_picker)

	add_child(row)
	add_focusable(_mode)

func _update_property() -> void:
	_updating = true
	var obj := get_edited_object()

	var mode: int = obj.get("mode")
	_mode.select(mode)

	var is_localised := mode == 0
	_key_picker.visible = is_localised
	_resource_picker.visible = not is_localised

	if is_localised:
		_key_picker.set_object_and_property(obj, "key")
		_key_picker._update_property()
	else:
		_resource_picker.edited_resource = obj.get("literal_sound")

	_updating = false

func _on_mode_selected(index: int) -> void:
	if _updating:
		return
	emit_changed("mode", index)

func _on_resource_changed(resource: Resource) -> void:
	if _updating:
		return
	emit_changed("literal_sound", resource)

func _relay_property_changed(property: StringName, value: Variant, field: StringName = "", changing: bool = false) -> void:
	if _updating:
		return
	emit_changed(property, value, field, changing)
