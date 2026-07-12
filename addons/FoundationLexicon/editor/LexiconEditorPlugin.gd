@tool
extends EditorPlugin

## Activates the Lexicon editor tooling: the compact LexiconText/LexiconAsset/
## LexiconSound inspector plugin, and the "Lexicon" bottom-panel dock. Enable via
## Project Settings > Plugins (this addon's plugin.cfg now points here).
##
## Gated: FoundationLexicon.gdextension (the native library everything here
## ultimately depends on — LexiconRegistry, the Subsystem integration, all
## of it) ships inert until heathen_gate confirms Godot-Game-Framework is
## actually installed. See gate/heathen_gate.gd and
## Godot-GameplayTags-Foundation's GameplayTagsEditorPlugin.gd (the
## reference implementation this mirrors) for the full mechanism and why.
##
## LexiconInspectorPlugin is loaded with a runtime load(), not a static
## type/preload — it does "is LexiconText"/"is LexiconAsset"/"is LexiconSound"
## checks internally, and GDScript resolves type identifiers at COMPILE time
## of whatever file references them. A plain typed var here would force
## GDScript to parse that whole file (and therefore resolve the native
## types) before the gate has a chance to run. LexiconDock has no such
## native-type reference of its own (checked), so it's safe to construct
## normally below.

const HeathenGate = preload("res://addons/FoundationLexicon/gate/heathen_gate.gd")

var _inspector_plugin: Object
var _dock: LexiconDock

func _enter_tree() -> void:
	if HeathenGate.ensure_unlocked(self, "FoundationLexicon", _activate_tooling):
		_activate_tooling()

# NOT named _build() — EditorPlugin already declares a virtual _build() -> bool
# (asks whether a custom pre-run build step should block "Run Project"); naming
# this the same collides with it and breaks script parsing entirely.
func _activate_tooling() -> void:
	var inspector_script: GDScript = load("res://addons/FoundationLexicon/editor/LexiconInspectorPlugin.gd")
	_inspector_plugin = inspector_script.new()
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
