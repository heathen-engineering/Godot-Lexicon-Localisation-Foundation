/*
 * Copyright (c) 2026 Heathen Engineering Limited
 * Irish Registered Company #556277
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

using Godot;

namespace Heathen.Lexicon
{
    public enum LexiconHintType
    {
        None = 0,
        String = 1,
        Sound = 2,
        Texture = 3,
        Sprite = 4,
        PackedScene = 5,
        Asset = 6,
    }

    /// <summary>
    /// Strongly-typed C# facade over the native "LexiconRegistry" engine singleton.
    /// Resolves strings and assets for the active culture through a fallback chain:
    /// exact culture -> base language -> default culture (+ its base) -> the
    /// culture-neutral "default" source.
    /// </summary>
    public static class LexiconRegistry
    {
        static GodotObject Instance => Engine.GetSingleton("LexiconRegistry");

        public static ulong Hash(string text) => (ulong)Instance.Call("hash", text);
        public static bool ValidateKey(string dotPath) => (bool)ClassDB.ClassCallStatic("LexiconRegistry", "validate_key", dotPath);

        /// <summary>Parses .helex JSON text and registers its entries.</summary>
        public static void RegisterSource(string helexJson) => Instance.Call("register_source", helexJson);
        public static void UnregisterAll() => Instance.Call("unregister_all");
        public static int RegisteredSourceCount => (int)Instance.Call("get_registered_source_count");

        public static void UseCulture(string cultureCode) => Instance.Call("use_culture", cultureCode);
        public static string ActiveCulture => (string)Instance.Call("get_active_culture");
        public static string DefaultCulture => (string)Instance.Call("get_default_culture");
        public static string[] GetMappedCultureCodes() => (string[])Instance.Call("get_mapped_culture_codes");
        public static string GetDisplayName(string assetId) => (string)Instance.Call("get_display_name", assetId);

        public static string ResolveString(ulong key) => (string)Instance.Call("resolve_string", key);
        public static string ResolveString(string dotPath) => (string)Instance.Call("resolve_string_path", dotPath);

        public static Resource ResolveAsset(ulong key)
        {
            Variant result = Instance.Call("resolve_asset", key);
            return result.Obj as Resource;
        }

        public static Resource ResolveAsset(string dotPath)
        {
            Variant result = Instance.Call("resolve_asset_path", dotPath);
            return result.Obj as Resource;
        }

        public static LexiconHintType GetHint(ulong key) => (LexiconHintType)(int)Instance.Call("get_hint", key);

        public static void SetString(string dotPath, string value, string cultureCode = "") =>
            Instance.Call("set_string", dotPath, value, cultureCode);

        public static void SetAsset(string dotPath, Resource asset, string cultureCode = "") =>
            Instance.Call("set_asset", dotPath, asset, cultureCode);

        public static void SetAssetPath(string dotPath, string path, LexiconHintType hint, string subName = "", string cultureCode = "") =>
            Instance.Call("set_asset_path", dotPath, path, (int)hint, subName, cultureCode);

        public static void RemoveKey(string dotPath, string cultureCode = "") => Instance.Call("remove_key", dotPath, cultureCode);

        /// <summary>Connect to be notified whenever the active culture changes.</summary>
        public static void ConnectCultureChanged(Callable callback) => Instance.Connect("culture_changed", callback);

        /// <summary>Connect to be notified whenever sources are (re)registered.</summary>
        public static void ConnectSourcesChanged(Callable callback) => Instance.Connect("sources_changed", callback);
    }
}
