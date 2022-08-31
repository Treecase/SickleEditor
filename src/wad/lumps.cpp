/**
 * lumps.cpp - WAD lump types.
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

#include "lumps.hpp"

#include <stdexcept>

#include <cstring>


WAD::TexLump WAD::readTexLump(Lump const &lump)
{
    if (lump.type != 0x43)
        throw std::runtime_error{"Expected lump.type = 0x43"};

    TexLump out{};

    auto const ptr0 = lump.data.data();
    auto ptr = ptr0;

    memcpy(out.name, ptr, 16);
    ptr += 16;

    memcpy(&out.width, ptr, 4);
    ptr += 4;
    memcpy(&out.height, ptr, 4);
    ptr += 4;

    uint32_t ptrs[4];
    memcpy(ptrs, ptr, 4 * 4);

    auto n = out.width * out.height;
    out.tex1.reserve(n);
    for (uint32_t i = 0; i < n; ++i)
        out.tex1.push_back(ptr0[ptrs[0] + i]);

    n = (out.width/2) * (out.height/2);
    out.tex2.reserve(n);
    for (uint32_t i = 0; i < n; ++i)
        out.tex2.push_back(ptr0[ptrs[1] + i]);

    n = (out.width/4) * (out.height/4);
    out.tex4.reserve(n);
    for (uint32_t i = 0; i < n; ++i)
        out.tex4.push_back(ptr0[ptrs[2] + i]);

    n = (out.width/8) * (out.height/8);
    out.tex8.reserve(n);
    for (uint32_t i = 0; i < n; ++i)
        out.tex8.push_back(ptr0[ptrs[3] + i]);

    ptr = ptr0 + ptrs[3] + n;

    uint16_t palsize;
    memcpy(&palsize, ptr, 2);
    ptr += 2;

    out.palette.reserve(palsize);
    for (uint16_t i = 0, j = 0; i < palsize; ++i, j += 3)
        out.palette.push_back({ptr[j+0], ptr[j+1], ptr[j+2]});

    return out;
}
