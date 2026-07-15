# Lexicon Localisation Foundation for Godot

![License](https://img.shields.io/badge/License-Apache_2.0-blue?style=flat-square)
![Maintained](https://img.shields.io/badge/Maintained%3F-yes-green?style=flat-square)
![Godot](https://img.shields.io/badge/Godot-4.6%20%2B-%23478CBF?style=flat-square&logo=godotengine&logoColor=white)

Culture-aware string and asset localisation for Godot 4, with a three-mode fallback chain per field (Localised / Literal / Invariant), keyed by xxHash3-64 dot-paths so lookups cost nothing at runtime once registered.

- **License:** Apache 2.0
- **Origin:** Heathen Group
- **Platforms:** Windows, Linux, macOS

> [!TIP]
> **Looking for the easiest way to install?**
> Copy `addons/FoundationLexicon/` straight into your project's `addons/` folder. There's no external package manager step. See [Install](#install) below.

---

## 🛠 Also Available For

[![O3DE](https://img.shields.io/badge/O3DE-25.10%20%2B-%2300AEEF?style=for-the-badge&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI+PHBhdGggZmlsbD0id2hpdGUiIGQ9Ik0xMiAxTDEgNy40djkuMkwxMiAyM2wxMS02LjRWNy40TDEyIDF6bTkuMSAxNC45TDExLjUgMjEuM2wtOC42LTYuNFY4LjFsOC42LTYuNCA5LjEgNi40djYuOHpNMTEuNSA0LjZMMi45IDkuNnY0LjhsOC42IDUuMSA4LjYtNS4xVjkuNmwtOC42LTUuMHoiLz48L3N2Zz4=)](https://github.com/heathen-engineering/O3DE-Lexicon-Foundation)
[![Unity](https://img.shields.io/badge/Unity-6000%20%2B-black?style=for-the-badge&logo=unity&logoColor=white)](https://github.com/heathen-engineering/Unity-Lexicon-Localisation-Foundation)

> [!NOTE]
> This Godot port treats [Unity-Lexicon-Localisation-Foundation](https://github.com/heathen-engineering/Unity-Lexicon-Localisation-Foundation) as the gold-standard reference. It's the newer, more complete of the two prior implementations. The `.helex` JSON schema and fallback-resolution algorithm are ported from it directly; only the asset-addressing layer changes (see below).

---

## Requirements

- Godot **4.6** or compatible
- [godot-cpp](https://github.com/godotengine/godot-cpp), checked out locally, for building from source
- [Godot-xxHash](https://github.com/heathen-engineering/Godot-xxHash) (compiled in directly, see [Build](#build))
- [Godot-Game-Framework](https://github.com/heathen-engineering/Godot-Game-Framework), **enabled in the
  consuming project**. This is a runtime dependency, not a build-time one. If it's missing, enabling this
  plugin walks you through fetching it automatically via
  [Extension Resolver for Godot](https://github.com/heathen-engineering/Godot-Extension-Resolver);
  see that project's README for how the manifest-driven resolution works.

---

## Support

For general questions, help, and troubleshooting, join our [Discord](https://discord.gg/xmtRNkW7hW). Thousands of developers are there and can often help faster than waiting on a maintainer. Please use [GitHub Issues](https://github.com/heathen-engineering/Godot-Lexicon-Localisation-Foundation/issues) for a confirmed bug or a feature request that needs tracking, not general support questions.

---

## Become a GitHub Sponsor
[![Discord](https://img.shields.io/badge/Discord--1877F2?style=social&logo=discord)](https://discord.gg/xmtRNkW7hW)
[![GitHub followers](https://img.shields.io/github/followers/heathen-engineering?style=social)](https://github.com/heathen-engineering?tab=followers)

Support Heathen by becoming a [GitHub Sponsor](https://github.com/sponsors/heathen-engineering). Sponsorship directly funds the development and maintenance of free tools like this, as well as our game development [Knowledge Base](https://heathen.group/) and community on [Discord](https://discord.gg/xmtRNkW7hW).

Sponsors also get access to our private SourceRepo, which includes developer tools for O3DE, Unreal, Unity, and Godot.
Learn more or explore other ways to support @ [heathen.group/kb](https://heathen.group/kb/do-more/)

---

## What it does

Lexicon Foundation gives you a structured localisation system built on these core types:

| Type | Purpose |
|------|---------|
| `LexiconRegistry` | Engine singleton holding the per-culture entry tables and resolving the fallback chain |
| `LexiconText` | A `Resource` field wrapper for localisation-aware strings |
| `LexiconSound` | A `Resource` field wrapper for localisation-aware `AudioStream` references |
| `LexiconAsset` | A `Resource` field wrapper for any other localisation-aware `Resource` (textures, packed scenes, ...) |

Resolution follows a fallback chain: **exact active culture → base language of active culture (e.g. `"fr"` from `"fr-CA"`) → default culture (+ its base) → the culture-neutral `"default"` source.** Runtime-injected entries (`set_string`/`set_asset`/`set_asset_path`) survive re-registration and always take priority, so a save-game or debug override never gets clobbered by a source reload.

This is a core dependency for [Godot-Ogham-Storyteller-Foundation](https://github.com/heathen-engineering/Godot-Ogham-Storyteller-Foundation). Dialogue lines, speaker names, and voice-over/portrait references are authored as `LexiconText`/`LexiconSound`/`LexiconAsset` fields on graph nodes.

---

## Install

Copy `addons/FoundationLexicon/` into your project's `addons/` folder. Enable the plugin from **Project Settings → Plugins**.

List your project's `.helex` files in a `LexiconSourceList` resource (`helex_paths: PackedStringArray`), then add `FoundationAutoload.tscn` as an AutoLoad and assign your list(s) to its `source_lists` export. They're registered on `_ready()`, and the active culture is set from `OS.get_locale()`.

## Build

Requires [godot-cpp](https://github.com/godotengine/godot-cpp) and [Godot-xxHash](https://github.com/heathen-engineering/Godot-xxHash) checked out locally.

```bash
cmake -S addons/FoundationLexicon -B addons/FoundationLexicon/build \
  -DGODOT_CPP_PATH=/path/to/Godot-cpp \
  -DGODOT_XXHASH_PATH=/path/to/Godot-xxHash
cmake --build addons/FoundationLexicon/build
```

Output lands in `addons/FoundationLexicon/bin/`.

---

## `.helex` file format

Tags are defined in `.helex` files, UTF-8 JSON documents placed anywhere in the project source tree and registered via `register_source()`.

```json
{
  "assetId":    "UI.Strings",
  "registered": true,
  "cultures":   ["en-GB"],
  "entries": {
    "UI.Play": "Play",
    "UI.Logo": { "path": "res://sprites/logo.png", "sub": "", "hint": "Sprite" }
  }
}
```

| Field | Description |
|-------|-------------|
| `assetId` | Stable internal identifier for the source (e.g. `"UI.Strings"`). An `assetId` of `"default"` (case-insensitive) additionally registers the source's entries as the culture-neutral last-resort fallback tier. |
| `registered` | When `false`, the source is skipped entirely, useful for drafts. Defaults to `true`. |
| `cultures` | BCP 47 culture codes this source provides entries for (e.g. `"en-GB"`, `"fr-CA"`). |
| `entries` | Map of dot-path key → value. A plain JSON string registers a `LexiconText`-resolvable string entry. An object registers an asset entry: `path` (a `res://` path, loaded on demand via `ResourceLoader`), `sub` (optional sub-resource name), `hint` (`String`/`Sound`/`Texture`/`Sprite`/`PackedScene`/`Asset`, classifies the entry without loading it). |

> [!NOTE]
> This schema is a straight port of Unity's `.helex` format. The only change is the asset-addressing field. Unity uses an Addressables `guid`; O3DE uses a compiled `AZ::Uuid`; Godot uses `path`, because a `res://` path **is** Godot's native addressable identifier already. There's no separate GUID/streaming layer to maintain.

**Key naming:** any non-empty dot-path string; leading/trailing dots are rejected by `validate_key`.

---

## Usage overview

### Registering sources and switching culture

```gdscript
var registry := Engine.get_singleton("LexiconRegistry")
registry.register_source(FileAccess.open("res://loc/ui.helex", FileAccess.READ).get_as_text())
registry.use_culture("fr-CA")
```

### Resolving a string field

```gdscript
var line := LexiconText.new()
line.mode = LexiconText.MODE_LOCALISED
line.key_or_value = "UI.Play"
print(line.resolve()) # French string if registered, else "UI.Play" itself
```

### Resolving an asset field

```gdscript
var portrait := LexiconAsset.new()
portrait.mode = LexiconAsset.MODE_LOCALISED
portrait.key = "Character.MrsWilds.Portrait"
var tex: Texture2D = portrait.resolve()

var voice := LexiconSound.new()
voice.mode = LexiconSound.MODE_LOCALISED
voice.key = "Dialogue.Act1.Scene2.Line014"
var clip: AudioStream = voice.resolve()
```

### Runtime injection (save data, mod content, debug overrides)

```gdscript
registry.set_string("UI.Play", "Jouer", "fr-CA")
registry.set_asset_path("Character.MrsWilds.Portrait", "res://mods/portraits/wilds_alt.png",
    LexiconRegistry.HINT_TEXTURE)
```

**C#**: every type above has a matching wrapper class in `Heathen.Lexicon` (`LexiconRegistry`, `LexiconText`, `LexiconSound`, `LexiconAsset`) so C# code never touches `Engine.GetSingleton`/`Variant.Call` directly. See the `CSharp/` folder.

---

## Public API reference

### `LexiconRegistry` (engine singleton)

| Method | Description |
|--------|-------------|
| `hash(text)` | Hash a string to an id (xxHash3\_64bits, seed 0), same hash space as GameplayTags |
| `validate_key(dot_path)` *(static)* | Validate format without registering |
| `register_source(helex_json)` | Parse and register a `.helex` JSON source |
| `unregister_all()` | Full reset: every source, override, and culture |
| `get_registered_source_count()` | Diagnostics |
| `use_culture(code)` | Set the active culture; fires `culture_changed` |
| `get_active_culture()` / `get_default_culture()` | Current fallback-chain anchors |
| `get_mapped_culture_codes()` | Every culture with at least one registered entry |
| `get_display_name(asset_id)` | Resolves `"Language.{asset_id}"`, falling back to `asset_id` itself |
| `resolve_string(key)` / `resolve_string_path(dot_path)` | String resolution through the fallback chain |
| `resolve_asset(key)` / `resolve_asset_path(dot_path)` | Asset resolution: direct reference wins, else loads `asset_path` on demand |
| `get_hint(key)` | The `LexiconHintType` for a key, or `HINT_NONE` if unregistered |
| `set_string(dot_path, value, culture = "")` | Inject/overwrite a string entry (empty culture = active) |
| `set_asset(dot_path, resource, culture = "")` | Inject/overwrite a direct asset reference; hint inferred from the resource's class |
| `set_asset_path(dot_path, path, hint, sub_name = "", culture = "")` | Inject/overwrite a path-addressed (lazily loaded) asset entry |
| `remove_key(dot_path, culture = "")` | Remove a runtime-injected entry; empty culture removes from every culture |
| `culture_changed(culture_code)` *(signal)* | Fires on `use_culture` |
| `sources_changed` *(signal)* | Fires after `register_source`/`unregister_all` |

**`LexiconHintType` values:** `HINT_NONE`, `HINT_STRING`, `HINT_SOUND`, `HINT_TEXTURE`, `HINT_SPRITE`, `HINT_PACKED_SCENE`, `HINT_ASSET`.

### `LexiconText` / `LexiconSound` / `LexiconAsset` (`Resource`)

All three share the same shape: a `mode` plus either a dot-path key (Localised) or a literal value:

| Property / Method | Description |
|--------------------|-------------|
| `mode` | `MODE_LOCALISED` / `MODE_LITERAL` / `MODE_INVARIANT` |
| `key_or_value` (`LexiconText`) / `key` (`LexiconSound`, `LexiconAsset`) | Dot-path key when Localised; ignored otherwise |
| `key_or_value` also holds the literal string directly (`LexiconText`) when Literal/Invariant | N/A |
| `literal_sound` (`LexiconSound`) / `literal_asset` (`LexiconAsset`) | The literal `Resource` value when Literal/Invariant, and the fallback used in Localised mode if unregistered |
| `is_localised()` / `is_literal()` / `is_invariant()` | Mode checks |
| `get_hash()` | The id for the current key (0 if unset) |
| `resolve()` | The resolved `String`/`AudioStream`/`Resource` for the active culture, falling back to the literal value |

> [!NOTE]
> Unity splits this concept into six field types (`LexiconAsset`/`LexiconTexture`/`LexiconSprite`/`LexiconPrefab` in addition to `LexiconText`/`LexiconSound`) because each Unity serialised field needs its own C# type to get Inspector type-filtering. Godot's `PROPERTY_HINT_RESOURCE_TYPE` does that filtering job on a single exported `Resource` property, so `LexiconAsset` here covers everything that isn't text or audio.

---

## Editor tooling

Enable the plugin (Project Settings > Plugins) to get:
- **Key-picker Inspector field**: `LexiconText`/`LexiconAsset`/`LexiconSound`'s `key_or_value`/`key` fields get a compact editor: a Mode dropdown, and either a searchable Tree popup of every key across every `.helex` file in the project (Localised) or a plain text/`EditorResourcePicker` field for the literal value (Literal/Invariant). `LexiconKeyPickerProperty` is reusable by other addons as a plain global `class_name`.
- **"Lexicon" bottom-panel dock**: default/fallback culture and available-cultures list (validated against a ~148-entry BCP-47 `KnownCultures` table ported from O3DE), plus a key browser aggregating every `.helex` file project-wide, flagging (in red) any key missing an entry for a configured available culture. The closest Godot analog to Unity's `Project Settings > Subsystems > Localisation Lexicon` page.

## Not yet (fully) ported

- **Async streaming.** Unity's Addressables seam (`LexiconAssetLoader`, reference-counted `AcquireAsset`/`ReleaseAsset`) has no Godot equivalent yet. `resolve_asset` calls `ResourceLoader::load()` synchronously. `ResourceLoader.load_threaded_request` is a natural drop-in addition later if voice-over streaming windows become a real cost; nothing in the current design blocks it.
- **Gatherer, CSV import/export, binary-compiler build pipeline** (O3DE's `.helex` → `.lexicon` AssetBuilder step) are still deferred. The key-picker/dock above cover the highest-value authoring gap; these are secondary.
- **Burst string snapshots.** Unity's `GetStringSnapshot` (a `NativeHashMap` for Job System access) has no Godot equivalent need. The registry already runs natively inside the GDExtension.

## License

Apache 2.0.
