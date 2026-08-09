# Lexicon Localisation Foundation for Godot

Culture-aware string and asset localisation for Godot 4, with a three-mode fallback chain per field (Localised, Literal, Invariant), keyed by xxHash3-64 dot-paths so lookups cost nothing at runtime once registered.

## What it does

Built on four core types:

- LexiconRegistry: engine singleton holding the per-culture entry tables and resolving the fallback chain.
- LexiconText: a Resource field wrapper for localisation-aware strings.
- LexiconSound: a Resource field wrapper for localisation-aware AudioStream references.
- LexiconAsset: a Resource field wrapper for any other localisation-aware Resource (textures, packed scenes, and more).

Resolution follows a fallback chain: exact active culture, then base language of the active culture, then default culture, then a culture-neutral default source. Runtime-injected entries always take priority, so a save-game or debug override never gets clobbered by a source reload.

Tag content lives in .helex files: plain UTF-8 JSON, readable and diffable, placed anywhere in your project source tree.

## Editor tooling

- Key-picker Inspector field: LexiconText, LexiconAsset, and LexiconSound fields get a compact editor with a Mode dropdown and a searchable key tree.
- Lexicon settings dock: default and available cultures list, validated against a BCP-47 known-cultures table, plus a key browser that flags any key missing an entry for a configured culture.

## Works from both GDScript and C#

A matching C# wrapper class exists for every runtime type, so C# code never touches Engine.GetSingleton or Variant.Call directly.

## Requirements

- Godot 4.6 or compatible
- [Godot Game Framework](https://github.com/heathen-engineering/Godot-Game-Framework), enabled in the consuming project. If it is missing, enabling this plugin walks you through fetching it automatically.

## Links

- GitHub: [https://github.com/heathen-engineering/Godot-Lexicon-Localisation-Foundation](https://github.com/heathen-engineering/Godot-Lexicon-Localisation-Foundation)
- Support and Discord: [https://discord.gg/xmtRNkW7hW](https://discord.gg/xmtRNkW7hW)
- License: Apache 2.0
