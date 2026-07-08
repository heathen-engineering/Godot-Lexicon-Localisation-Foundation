@tool
extends EditorProperty
class_name LexiconTextCompactEditor

## One-row compact editor for LexiconText: Mode | (key picker when Localised, or a
## plain LineEdit for the literal string when Literal/Invariant) — both read/write
## the same underlying key_or_value field, just with a different editor depending on
## mode. Bound to LexiconText's full property set via
## add_property_editor_for_multiple_properties (see LexiconInspectorPlugin.gd).

const MODES := ["Localised", "Literal", "Invariant"]

var _mode: OptionButton
var _key_picker: LexiconKeyPickerProperty
var _literal_edit: LineEdit
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

	_literal_edit = LineEdit.new()
	_literal_edit.custom_minimum_size = Vector2(200, 0)
	_literal_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_literal_edit.text_submitted.connect(_on_literal_submitted)
	_literal_edit.focus_exited.connect(func(): _on_literal_submitted(_literal_edit.text))
	row.add_child(_literal_edit)

	add_child(row)
	add_focusable(_mode)

func _update_property() -> void:
	_updating = true
	var obj := get_edited_object()

	var mode: int = obj.get("mode")
	_mode.select(mode)

	var is_localised := mode == 0
	_key_picker.visible = is_localised
	_literal_edit.visible = not is_localised

	if is_localised:
		_key_picker.set_object_and_property(obj, "key_or_value")
		_key_picker._update_property()
	else:
		_literal_edit.text = obj.get("key_or_value")

	_updating = false

func _on_mode_selected(index: int) -> void:
	if _updating:
		return
	emit_changed("mode", index)

func _on_literal_submitted(text: String) -> void:
	if _updating:
		return
	emit_changed("key_or_value", text)

func _relay_property_changed(property: StringName, value: Variant, field: StringName = "", changing: bool = false) -> void:
	if _updating:
		return
	emit_changed(property, value, field, changing)
