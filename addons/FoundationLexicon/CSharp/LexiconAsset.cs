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
    /// <summary>
    /// Wrapper over a native LexiconAsset (Resource). A localisation-aware generic Resource
    /// field (textures, packed scenes, ...) — resolves via LexiconRegistry when Localised,
    /// otherwise returns the literal asset.
    /// </summary>
    public class LexiconAsset
    {
        private readonly Resource _instance;

        public Resource ToGDNative() => _instance;

        public LexiconAsset() => _instance = (Resource)ClassDB.Instantiate("LexiconAsset");
        public LexiconAsset(Resource instance) => _instance = instance;

        public LexiconLocMode Mode
        {
            get => (LexiconLocMode)(int)_instance.Get("mode");
            set => _instance.Set("mode", (int)value);
        }

        public string Key
        {
            get => (string)_instance.Get("key");
            set => _instance.Set("key", value);
        }

        public Resource LiteralAsset
        {
            get => _instance.Get("literal_asset").Obj as Resource;
            set => _instance.Set("literal_asset", value);
        }

        public bool IsLocalised => (bool)_instance.Call("is_localised");
        public bool IsLiteral => (bool)_instance.Call("is_literal");
        public bool IsInvariant => (bool)_instance.Call("is_invariant");

        public ulong GetHash() => (ulong)_instance.Call("get_hash");

        public Resource Resolve()
        {
            Variant result = _instance.Call("resolve");
            return result.Obj as Resource;
        }
    }
}
