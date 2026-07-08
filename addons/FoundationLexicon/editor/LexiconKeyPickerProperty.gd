@tool
extends EditorProperty
class_name LexiconKeyPickerProperty

## Inspector field for a Lexicon dot-path key. Same Tree+filter-popup pattern as
## GameplayTagPickerProperty (FoundationGameplayTags), built independently here (no
## compile-time dependency between the two addons) since each Foundation extension
## must stay loadable standalone. Shows per-key culture coverage as a hint alongside
## each entry so a translator/designer can see gaps at a glance.

var _button: Button
var _popup: PopupPanel
var _filter: LineEdit
var _tree: Tree
var _updating := false

func _init() -> void:
	_button = Button.new()
	_button.alignment = HORIZONTAL_ALIGNMENT_LEFT
	_button.pressed.connect(_open_popup)
	add_child(_button)
	add_focusable(_button)

	_popup = PopupPanel.new()
	var vbox := VBoxContainer.new()
	_popup.add_child(vbox)

	_filter = LineEdit.new()
	_filter.placeholder_text = "Filter keys..."
	_filter.custom_minimum_size = Vector2(320, 0)
	_filter.text_changed.connect(_on_filter_changed)
	vbox.add_child(_filter)

	_tree = Tree.new()
	_tree.custom_minimum_size = Vector2(320, 320)
	_tree.hide_root = true
	_tree.item_activated.connect(_on_tree_item_activated)
	vbox.add_child(_tree)

	add_child(_popup)

func _update_property() -> void:
	var value = get_edited_object().get(get_edited_property())
	_updating = true
	_button.text = value if not String(value).is_empty() else "(none)"
	_updating = false

func _open_popup() -> void:
	_filter.text = ""
	_rebuild_tree("")
	_popup.popup_centered(Vector2(340, 360))
	_filter.grab_focus()

func _on_filter_changed(new_text: String) -> void:
	_rebuild_tree(new_text)

func _rebuild_tree(filter: String) -> void:
	_tree.clear()
	var root := _tree.create_item()
	var all_keys := LexiconVocabulary.get_all_keys()
	var filter_lower := filter.to_lower()
	var names := all_keys.keys()
	names.sort()
	for key in names:
		if not filter.is_empty() and not String(key).to_lower().contains(filter_lower):
			continue
		var info: LexiconVocabulary.KeyInfo = all_keys[key]
		var item := _tree.create_item(root)
		item.set_text(0, "%s  [%s]" % [key, ", ".join(info.cultures)])
		item.set_metadata(0, key)

func _on_tree_item_activated() -> void:
	var selected := _tree.get_selected()
	if selected == null:
		return
	var key: String = selected.get_metadata(0)
	_popup.hide()
	emit_changed(get_edited_property(), key)
