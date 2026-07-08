@tool
extends Resource
class_name LexiconSettings

## Project-wide Lexicon settings — persisted at res://LexiconSettings.tres (a
## project-owned resource, edited via LexiconDock). The Godot analog of Unity's
## LexiconSettingsProvider (Project Settings > Subsystems > Localisation Lexicon).

@export var default_culture: String = "en-GB"
@export var available_cultures: Array[String] = []
