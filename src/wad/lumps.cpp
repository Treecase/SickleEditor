/**
 * lumps.cpp - WAD lump types.
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

#include "wad/lumps.hpp"

#include <cstring>

using namespace WAD;


TexLumpLoadError::TexLumpLoadError(Lump const &lump, std::string const &what)
:   std::runtime_error{lump.name + ": " + what}
,   name{lump.name}
{
}



TexLump WAD::readTexLump(Lump const &lump)
{
    return TexLump{lump};
}



TexLump::TexLump(Lump const &src)
:   _src{std::make_shared<Lump>(src)}
,   _cached{std::make_shared<DataCache>()}
{
    if (_src->type != 0x43)
        throw TexLumpLoadError{*_src, "lump type is not 0x43"};

    auto ptr = _src->data.data();

    char name[16];
    memcpy(name, ptr, 16);
    _name = std::string{name, 16};

    memcpy(&_width, ptr + 16, 4);
    memcpy(&_height, ptr + 20, 4);

    memcpy(_ptrs.data(), ptr + 24, 4 * 4);
}


std::string TexLump::name() const
{
    return _name;
}


uint32_t TexLump::width() const
{
    return _width;
}


uint32_t TexLump::height() const
{
    return _height;
}


std::vector<uint8_t> TexLump::tex1() const
{
    if (!_cached->tex1.has_value())
    {
        auto const ptr = _src->data.data() + _ptrs.at(0);
        auto const n = _width * _height;
        _cached->tex1.emplace(ptr, ptr + n);
    }
    return _cached->tex1.value();
}


std::vector<uint8_t> TexLump::tex2() const
{
    if (!_cached->tex2.has_value())
    {
        auto const ptr = _src->data.data() + _ptrs.at(1);
        auto const n = (_width / 2) * (_height / 2);
        _cached->tex2.emplace(ptr, ptr + n);
    }
    return _cached->tex2.value();
}


std::vector<uint8_t> TexLump::tex4() const
{
    if (!_cached->tex4.has_value())
    {
        auto const ptr = _src->data.data() + _ptrs.at(2);
        auto const n = (_width / 4) * (_height / 4);
        _cached->tex4.emplace(ptr, ptr + n);
    }
    return _cached->tex4.value();
}


std::vector<uint8_t> TexLump::tex8() const
{
    if (!_cached->tex8.has_value())
    {
        auto const ptr = _src->data.data() + _ptrs.at(3);
        auto const n = (_width / 8) * (_height / 8);
        _cached->tex8.emplace(ptr, ptr + n);
    }
    return _cached->tex8.value();
}


std::vector<std::array<uint8_t, 3>> TexLump::palette() const
{
    if (!_cached->palette.has_value())
    {
        auto const ptr = (
            _src->data.data()
            + _ptrs.at(3)
            + ((_width / 8) * (_height / 8)));

        uint16_t palsize;
        memcpy(&palsize, ptr, 2);

        auto &palette = _cached->palette.emplace();
        for (uint16_t i = 0; i < palsize; ++i)
        {
            auto const j = i * 3;
            palette.push_back({ptr[2 + j + 0], ptr[2 + j + 1], ptr[2 + j + 2]});
        }
    }
    return _cached->palette.value();
}
