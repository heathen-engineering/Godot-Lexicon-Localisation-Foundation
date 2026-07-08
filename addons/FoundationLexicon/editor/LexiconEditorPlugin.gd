@tool
extends EditorPlugin

## Activates the Lexicon editor tooling: the compact LexiconText/LexiconAsset/
## LexiconSound inspector plugin, and the "Lexicon" bottom-panel dock. Enable via
## Project Settings > Plugins (this addon's plugin.cfg now points here).

var _inspector_plugin: LexiconInspectorPlugin
var _dock: LexiconDock

func _enter_tree() -> void:
	_inspector_plugin = LexiconInspectorPlugin.new()
	add_inspector_plugin(_inspector_plugin)

	_dock = LexiconDock.new()
	add_control_to_bottom_panel(_dock, "Lexicon")

func _exit_tree() -> void:
	if _inspector_plugin != null:
		remove_inspector_plugin(_inspector_plugin)
		_inspector_plugin = null
	if _dock != null:
		remove_control_from_bottom_panel(_dock)
		_dock.queue_free()
		_dock = null
