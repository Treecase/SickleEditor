/**
 * wad.hpp - Load WAD files.
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

#ifndef SE_LOAD_WAD
#define SE_LOAD_WAD

#include <string>
#include <vector>

#include <cstdint>


namespace WAD
{
    /** Container for data stored in the WAD. */
    struct Lump
    {
        // Lump name.
        std::string name;
        // Lump type.
        uint8_t type;
        // Data contained by the Lump.
        std::vector<uint8_t> data;
    };

    /** Loaded WAD data. */
    struct WAD
    {
        // WAD version. (Half-Life seems to only use WAD3?)
        char id[4];
        // List of Lumps in the WAD.
        std::vector<Lump> directory;
    };

    /** Load a WAD file. */
    WAD load(std::string const &path);

    /** Print WAD data. */
    void print(WAD const &wad);
}

#endif
