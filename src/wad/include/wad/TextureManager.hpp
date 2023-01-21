/**
 * TextureManager.hpp - Lazy-loading WAD texture manager.
 * Copyright (C) 2022 Trevor Last
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
    template<typename LoadedTexture>
    struct TextureManager
    {
        std::unordered_map<std::string, Lump> lumps{};
        std::unordered_map<std::string, LoadedTexture> textures;

        TextureManager()
        :   lumps{}
        ,   textures{}
        {
        }
        TextureManager(WAD const &wad)
        :   TextureManager()
        {
            add_wad(wad);
        }

        /** Add a WAD to the manager. */
        void add_wad(WAD const &wad)
        {
            for (auto const &lump : wad.directory)
                if (lump.type == 0x43)
                    lumps.emplace(lump.name, lump);
        }
        /** Same as `textures.at(key)`, unless this would fail, in which case
         *  attempt to load the lump identified by `key` from the WAD. */
        LoadedTexture &at(std::string const &key)
        {
            try
            {
                return textures.at(key);
            }
            catch (std::out_of_range const &)
            {
                return textures[key] = LoadedTexture{
                    readTexLump(lumps.at(key))};
            }
        }
    };
}

#endif
