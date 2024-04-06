/**
 * LumpTexture.cpp - WAD3 texture lump.
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

#include "LumpTexture.hpp"


using namespace WAD;


static uint32_t u32_correct_endian(uint8_t const bytes[4])
{
    uint32_t const integer = (
        bytes[0]
        | (bytes[1] << 8)
        | (bytes[2] << 16)
        | (bytes[3] << 24));
    return integer;
}

static uint16_t u16_correct_endian(uint8_t const bytes[2])
{
    uint16_t const integer = bytes[0] | (bytes[1] << 8);
    return integer;
}



LumpTexture::LumpTexture(
    WADReader::DirectoryEntry const &entry,
    uint8_t const *bytes)
:   _name{entry.name}
{
    if (entry.type != 0x43)
        throw BadTypeException{"expected lump type 0x43"};

    // TODO: Would be good to have some bounds safety here instead of just
    // trusting the pointer.
    _texture_name = std::string{bytes, bytes + 16};

    _width = u32_correct_endian(bytes + 16);
    _height = u32_correct_endian(bytes + 20);

    // Load textures.
    for (decltype(_textures)::size_type i = 0; i < _textures.size(); ++i)
    {
        auto const tex_offset = u32_correct_endian(bytes + 24 + 4 * i);
        auto const tex_ptr = bytes + tex_offset;

        auto const width = _width / (1 << i);
        auto const height = _height / (1 << i);

        auto &texdata = _textures.at(i);
        texdata.insert(texdata.end(), tex_ptr, tex_ptr + width * height);
    }

    // Load palette.
    auto const final_tex_offset =\
        u32_correct_endian(bytes + 24 + 4 * (_textures.size() - 1));
    auto const final_width = _width / (1 << (_textures.size() - 1));
    auto const final_height = _height / (1 << (_textures.size() - 1));

    auto const palette_offset = final_tex_offset + final_width * final_height;
    auto const palette_ptr = bytes + palette_offset;

    auto const palette_size = u16_correct_endian(palette_ptr);
    for (uint16_t i = 0; i < palette_size; ++i)
    {
        _palette.push_back({
            (palette_ptr + 2 + 3 * i)[0],
            (palette_ptr + 2 + 3 * i)[1],
            (palette_ptr + 2 + 3 * i)[2]});
    }
}


std::string LumpTexture::name() const
{
    return _name;
}


std::string LumpTexture::texture_name() const
{
    return _texture_name;
}


uint32_t LumpTexture::width() const
{
    return _width;
}


uint32_t LumpTexture::height() const
{
    return _height;
}


std::vector<uint8_t> const &LumpTexture::tex1() const
{
    return _textures.at(0);
}


std::vector<uint8_t> const &LumpTexture::tex2() const
{
    return _textures.at(1);
}


std::vector<uint8_t> const &LumpTexture::tex4() const
{
    return _textures.at(2);
}


std::vector<uint8_t> const &LumpTexture::tex8() const
{
    return _textures.at(3);
}


std::vector<std::array<uint8_t, 3>> const &LumpTexture::palette() const
{
    return _palette;
}
