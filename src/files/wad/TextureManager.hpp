/**
 * TextureManager.hpp - Lazy-loading WAD texture manager.
 * Copyright (C) 2022-2023 Trevor Last
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SE_TEXTUREMANAGER_HPP
#define SE_TEXTUREMANAGER_HPP

#include "wad.hpp"
#include "lumps.hpp"

#include <sigc++/signal.h>

#include <stdexcept>
#include <string>
#include <unordered_map>


namespace WAD
{
    /**
     * Lazy-loading texture manager. Grabbing all the textures from the WAD is
     * really slow, and we usually only need a few textures, so instead we only
     * load from WAD when we need to.
     *
     * LoadedTexture must have a constructor that takes a TexLump as the sole
     * argument.
     */
    class TextureManager
    {
        static sigc::signal<void(std::string)> _sig_texlump_load_error;
        std::unordered_map<std::string, TexLump> _textures{};

        TextureManager();
        TextureManager(TextureManager const &)=delete;
        TextureManager(TextureManager &&)=delete;

    public:
        std::unordered_map<std::string, Lump> lumps{};

        /** Emitted if an error is encountered while reading a TexLump. */
        static auto &signal_texlump_load_error() {
            return _sig_texlump_load_error;}

        /** Get a reference to the TextureManager singleton. */
        static TextureManager &get_reference();

        /** Add a WAD to the manager. */
        void add_wad(WAD const &wad);

        /**
         * Get the texture lump identified by KEY.
         *
         * If the lump has not already been loaded from the WAD, it will be
         * loaded. If it has, the cached object will be returned.
         *
         * If a WAD read error is encountered, signal_texlump_read_error will be
         * emitted.
         */
        TexLump &at(std::string const &key);
    };


    template<typename ProxyType>
    struct TextureManagerProxy
    {
        std::unordered_map<std::string, ProxyType> textures{};

        static TextureManagerProxy &create()
        {
            static TextureManagerProxy proxy{};
            return proxy;
        }

        ProxyType &at(std::string const &key)
        {
            try {
                return textures.at(key);
            }
            catch (std::out_of_range const &) {
                auto &texman = TextureManager::get_reference();
                auto kv = textures.emplace(key, texman.at(key));
                return kv.first->second;
            }
        }

    private:
        TextureManagerProxy()
        {
        }
        TextureManagerProxy(TextureManagerProxy const &)=delete;
        TextureManagerProxy(TextureManagerProxy &&)=delete;
    };
}

#endif
