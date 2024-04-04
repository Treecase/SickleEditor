/**
 * TextureManager.cpp - Lazy-loading texture manager.
 * Copyright (C) 2024 Trevor Last
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

#include "TextureManager.hpp"

#include <files/wad/wad.hpp>
#include <files/wad/lumps.hpp>

#include <iostream>


using namespace Sickle::Editor::Textures;


sigc::signal<void> TextureManager::_sig_wads_changed{};


TextureManager &TextureManager::get_reference()
{
    static TextureManager the_instance{};
    return the_instance;
}



TextureManager::TextureManager()
{
}


void TextureManager::add_wad(std::filesystem::path const &wad_path)
{
    auto const wad_name = wad_path.stem().generic_string();
    auto const wad = WAD::load(wad_path);
    std::vector<std::shared_ptr<TextureInfo>> wad_textures{};
    for (auto const &lump : wad.directory)
    {
        WAD::TexLump texlump;
        try {
            texlump = WAD::readTexLump(lump);
        }
        catch (WAD::TexLumpLoadError const &e) {
            std::cerr << "Texture Load Error: " << e.what() << std::endl;
            continue;
        }
        std::shared_ptr<TextureInfo> texture_info{
            new TextureInfo{wad_name, texlump}};
        wad_textures.push_back(texture_info);
        _textures.insert(texture_info);
        _by_name.insert({texture_info->get_name(), texture_info});
    }
    _by_wad.insert({wad_name, wad_textures});
    signal_wads_changed().emit();
}


void TextureManager::remove_wad(std::string const &wad_name)
{
    for (auto const &texture : _by_wad.at(wad_name))
    {
        _textures.erase(texture);
        _by_name.erase(texture->get_name());
    }
    _by_wad.erase(wad_name);
    signal_wads_changed().emit();
}


void TextureManager::clear_wads()
{
    _textures.clear();
    _by_wad.clear();
    _by_name.clear();
    signal_wads_changed().emit();
}


std::unordered_set<std::string> TextureManager::get_wads() const
{
    std::unordered_set<std::string> wads{};
    for (auto const &kv : _by_wad)
        wads.insert(kv.first);
    return wads;
}


std::shared_ptr<TextureInfo> TextureManager::get_texture(
    std::string const &name) const
{
    return _by_name.at(name);
}
