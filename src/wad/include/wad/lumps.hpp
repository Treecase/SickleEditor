/**
 * lumps.hpp - WAD lump types.
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

#ifndef SE_LUMPS_HPP
#define SE_LUMPS_HPP

#include "wad.hpp"

#include <array>
#include <vector>

#include <cstdint>


namespace WAD
{
    /**
     * Texture lump. Type 0x43.
     */
    struct TexLump
    {
        char name[16];
        uint32_t width, height;
        std::vector<uint8_t> tex1;
        std::vector<uint8_t> tex2;
        std::vector<uint8_t> tex4;
        std::vector<uint8_t> tex8;
        std::vector<std::array<uint8_t, 3>> palette;
    };

    /* Read a TexLump from a Lump. */
    TexLump readTexLump(Lump const &lump);
}

#endif
