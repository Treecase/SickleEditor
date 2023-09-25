/**
 * TextureManager.cpp - Lazy-loading WAD texture manager.
 * Copyright (C) 2023 Trevor Last
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

#include "wad/TextureManager.hpp"

using namespace WAD;


sigc::signal<void(std::string)> TextureManager::_sig_texlump_load_error{};


TextureManager::TextureManager()
{
}


TextureManager &TextureManager::get_reference()
{
    static TextureManager texman{};
    return texman;
}


void TextureManager::add_wad(WAD const &wad)
{
    for (auto const &lump : wad.directory)
        if (lump.type == 0x43)
            lumps.emplace(lump.name, lump);
}


TexLump &TextureManager::at(std::string const &key)
{
    try {
        return _textures.at(key);
    }
    catch (std::out_of_range const &) {
        try {
            return _textures[key] = readTexLump(lumps.at(key));
        }
        catch (TexLumpLoadError const &e) {
            signal_texlump_load_error().emit(e.what());
            throw e;
        }
    }
}
