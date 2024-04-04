/**
 * TextureInfo.cpp - Holds texture information.
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

#include "TextureInfo.hpp"

#include <functional>


using namespace Sickle::Editor::Textures;


using GetMipmapDataFunc =\
    std::function<std::vector<uint8_t>(WAD::TexLump const *)>;

static std::unordered_map<MipmapLevel, GetMipmapDataFunc> const
GET_MIPMAP_DATA_FUNCS{
    {MipmapLevel::MIPMAP_FULL, std::mem_fn(&WAD::TexLump::tex1)},
    {MipmapLevel::MIPMAP_HALF, std::mem_fn(&WAD::TexLump::tex2)},
    {MipmapLevel::MIPMAP_QUARTER, std::mem_fn(&WAD::TexLump::tex4)},
    {MipmapLevel::MIPMAP_EIGHTH, std::mem_fn(&WAD::TexLump::tex8)},
};



TextureInfo::TextureInfo(
    std::string source_wad,
    WAD::TexLump const &texlump)
:   _source_wad{source_wad}
,   _texlump{texlump}
{
}


std::string TextureInfo::get_source_wad() const
{
    return _source_wad;
}


std::string TextureInfo::get_name() const
{
    return _texlump.name();
}


unsigned int TextureInfo::get_width(MipmapLevel mipmap) const
{
    return _texlump.width() / (1 << static_cast<int>(mipmap));
}


unsigned int TextureInfo::get_height(MipmapLevel mipmap) const
{
    return _texlump.height() / (1 << static_cast<int>(mipmap));
}


std::shared_ptr<uint8_t[]> TextureInfo::load_rgba(MipmapLevel mipmap) const
{
    auto const &palette = _texlump.palette();
    auto const tex = std::invoke(GET_MIPMAP_DATA_FUNCS.at(mipmap), &_texlump);

    auto const texture_size = get_width(mipmap) * get_height(mipmap);
    std::shared_ptr<uint8_t[]> buffer{
        new uint8_t[texture_size * 4]};

    for (size_t i = 0; i < texture_size; ++i)
    {
        auto const &rgb = palette.at(tex.at(i));
        buffer[(i*4)+0] = rgb.at(0);
        buffer[(i*4)+1] = rgb.at(1);
        buffer[(i*4)+2] = rgb.at(2);
        buffer[(i*4)+3] = 0xff;
    }

    return buffer;
}


std::shared_ptr<uint8_t[]> TextureInfo::load_rgb(MipmapLevel mipmap) const
{
    auto const &palette = _texlump.palette();
    auto const tex = std::invoke(GET_MIPMAP_DATA_FUNCS.at(mipmap), &_texlump);

    auto const texture_size = get_width(mipmap) * get_height(mipmap);
    std::shared_ptr<uint8_t[]> buffer{
        new uint8_t[texture_size * 3]};

    for (size_t i = 0; i < texture_size; ++i)
    {
        auto const &rgb = palette.at(tex.at(i));
        buffer[(i*3)+0] = rgb.at(0);
        buffer[(i*3)+1] = rgb.at(1);
        buffer[(i*3)+2] = rgb.at(2);
    }

    return buffer;
}
