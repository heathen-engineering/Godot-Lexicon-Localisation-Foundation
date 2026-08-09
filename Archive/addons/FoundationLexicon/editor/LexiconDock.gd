@tool
extends Control
class_name LexiconDock

## Bottom-panel dock: settings (default culture, available cultures) plus a key
## browser Tree aggregating every ".helex" file project-wide, flagging keys missing
## an entry for any configured available culture. The Godot analog of Unity's
## LexiconSettingsProvider / O3DE's LexiconToolWindow.

const SETTINGS_PATH := "res://LexiconSettings.tres"
const MISSING_COLOR := Color(1.0, 0.55, 0.45)
const COMPLETE_COLOR := Color(0.55, 0.85, 0.55)

var _settings: LexiconSettings
var _default_culture_edit: LineEdit
var _available_list: ItemList
var _add_culture_edit: LineEdit
var _key_tree: Tree
var _culture_popup: PopupPanel
var _culture_filter: LineEdit
var _culture_list_popup: ItemList
var _culture_popup_target: LineEdit

func _ready() -> void:
	name = "Lexicon"
	_load_or_create_settings()
	_build_culture_popup()

	var root_vbox := VBoxContainer.new()
	root_vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(root_vbox)

	var settings_row := HBoxContainer.new()
	var default_label := Label.new()
	default_label.text = "Default Culture:"
	settings_row.add_child(default_label)
	_default_culture_edit = LineEdit.new()
	_default_culture_edit.text = _settings.default_culture
	_default_culture_edit.custom_minimum_size = Vector2(100, 0)
	_default_culture_edit.text_submitted.connect(_on_default_culture_submitted)
	_default_culture_edit.focus_exited.connect(func(): _on_default_culture_submitted(_default_culture_edit.text))
	settings_row.add_child(_default_culture_edit)
	var default_pick_btn := Button.new()
	default_pick_btn.text = "..."
	default_pick_btn.pressed.connect(func(): _open_culture_popup(_default_culture_edit))
	settings_row.add_child(default_pick_btn)
	var refresh_btn := Button.new()
	refresh_btn.text = "Refresh Keys"
	refresh_btn.pressed.connect(_rebuild_key_tree)
	settings_row.add_child(refresh_btn)
	root_vbox.add_child(settings_row)

	var split := HSplitContainer.new()
	split.size_flags_vertical = Control.SIZE_EXPAND_FILL
	root_vbox.add_child(split)

	var cultures_vbox := VBoxContainer.new()
	cultures_vbox.custom_minimum_size = Vector2(220, 0)
	var cultures_label := Label.new()
	cultures_label.text = "Available Cultures"
	cultures_vbox.add_child(cultures_label)
	_available_list = ItemList.new()
	_available_list.size_flags_vertical = Control.SIZE_EXPAND_FILL
	for c in _settings.available_cultures:
		_available_list.add_item(c)
	cultures_vbox.add_child(_available_list)
	var cultures_add_row := HBoxContainer.new()
	_add_culture_edit = LineEdit.new()
	_add_culture_edit.placeholder_text = "fr-FR"
	_add_culture_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_add_culture_edit.text_submitted.connect(func(_t): _on_add_culture_pressed())
	cultures_add_row.add_child(_add_culture_edit)
	var add_culture_pick_btn := Button.new()
	add_culture_pick_btn.text = "..."
	add_culture_pick_btn.pressed.connect(func(): _open_culture_popup(_add_culture_edit))
	cultures_add_row.add_child(add_culture_pick_btn)
	var add_culture_btn := Button.new()
	add_culture_btn.text = "Add"
	add_culture_btn.pressed.connect(_on_add_culture_pressed)
	cultures_add_row.add_child(add_culture_btn)
	var delete_culture_btn := Button.new()
	delete_culture_btn.text = "Delete"
	delete_culture_btn.pressed.connect(_on_delete_culture_pressed)
	cultures_add_row.add_child(delete_culture_btn)
	cultures_vbox.add_child(cultures_add_row)
	split.add_child(cultures_vbox)

	var keys_vbox := VBoxContainer.new()
	keys_vbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var keys_label := Label.new()
	keys_label.text = "Keys (across every .helex file) — red = missing a configured culture"
	keys_vbox.add_child(keys_label)
	_key_tree = Tree.new()
	_key_tree.hide_root = true
	_key_tree.size_flags_vertical = Control.SIZE_EXPAND_FILL
	keys_vbox.add_child(_key_tree)
	split.add_child(keys_vbox)

	_rebuild_key_tree()

func _build_culture_popup() -> void:
	_culture_popup = PopupPanel.new()
	var vbox := VBoxContainer.new()
	_culture_popup.add_child(vbox)
	_culture_filter = LineEdit.new()
	_culture_filter.placeholder_text = "Filter cultures..."
	_culture_filter.custom_minimum_size = Vector2(260, 0)
	_culture_filter.text_changed.connect(_on_culture_filter_changed)
	vbox.add_child(_culture_filter)
	_culture_list_popup = ItemList.new()
	_culture_list_popup.custom_minimum_size = Vector2(260, 300)
	_culture_list_popup.item_activated.connect(_on_culture_popup_activated)
	vbox.add_child(_culture_list_popup)
	add_child(_culture_popup)

func _open_culture_popup(target: LineEdit) -> void:
	_culture_popup_target = target
	_culture_filter.text = ""
	_rebuild_culture_popup_list("")
	_culture_popup.popup_centered(Vector2(280, 340))
	_culture_filter.grab_focus()

func _on_culture_filter_changed(text: String) -> void:
	_rebuild_culture_popup_list(text)

func _rebuild_culture_popup_list(filter: String) -> void:
	_culture_list_popup.clear()
	var filter_lower := filter.to_lower()
	for code in KnownCultures.get_all_codes():
		var display := "%s — %s" % [code, KnownCultures.get_culture_name(code)]
		if not filter.is_empty() and not display.to_lower().contains(filter_lower):
			continue
		var idx := _culture_list_popup.add_item(display)
		_culture_list_popup.set_item_metadata(idx, code)

func _on_culture_popup_activated(index: int) -> void:
	var code: String = _culture_list_popup.get_item_metadata(index)
	_culture_popup.hide()
	if _culture_popup_target != null:
		_culture_popup_target.text = code
		if _culture_popup_target == _default_culture_edit:
			_on_default_culture_submitted(code)

func _on_default_culture_submitted(text: String) -> void:
	_settings.default_culture = text
	_save_settings()
	_rebuild_key_tree()

func _on_add_culture_pressed() -> void:
	var code := _add_culture_edit.text.strip_edges()
	if code.is_empty():
		return
	if not _settings.available_cultures.has(code):
		_settings.available_cultures.append(code)
		_available_list.add_item(code)
		_save_settings()
		_rebuild_key_tree()
	_add_culture_edit.text = ""

func _on_delete_culture_pressed() -> void:
	var selected := _available_list.get_selected_items()
	for i in range(selected.size() - 1, -1, -1):
		var idx: int = selected[i]
		_settings.available_cultures.remove_at(idx)
		_available_list.remove_item(idx)
	_save_settings()
	_rebuild_key_tree()

func _rebuild_key_tree() -> void:
	_key_tree.clear()
	var root := _key_tree.create_item()
	var keys := LexiconVocabulary.get_all_keys()
	var names := keys.keys()
	names.sort()
	for key in names:
		var info: LexiconVocabulary.KeyInfo = keys[key]
		var missing: Array = []
		for culture in _settings.available_cultures:
			if not info.cultures.has(culture) and not info.cultures.has("default"):
				missing.append(culture)
		var item := _key_tree.create_item(root)
		item.set_text(0, "%s  [%s]" % [key, ", ".join(info.cultures)])
		item.set_custom_color(0, MISSING_COLOR if missing.size() > 0 else COMPLETE_COLOR)
		if missing.size() > 0:
			item.set_tooltip_text(0, "Missing: " + ", ".join(missing))

func _load_or_create_settings() -> void:
	if ResourceLoader.exists(SETTINGS_PATH):
		_settings = load(SETTINGS_PATH)
	else:
		_settings = LexiconSettings.new()
		_save_settings()

func _save_settings() -> void:
	var err := ResourceSaver.save(_settings, SETTINGS_PATH)
	if err != OK:
		push_warning("LexiconDock: failed to save %s (error %d)" % [SETTINGS_PATH, err])
