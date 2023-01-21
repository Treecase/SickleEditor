/**
 * wad.cpp - Load WAD files.
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

#include "wad/wad.hpp"

#include <fstream>
#include <vector>

#include <cstring>


struct M_Header
{
    // "WAD3"
    char magic[4];
    // Number of entries in directory.
    uint32_t numlumps;
    // Pointer to start of directory.
    uint32_t diroffset;
};

struct M_Lump
{
    // Pointer to start of lump data.
    uint32_t offset;
    // In-WAD size of lump.
    uint32_t dsize;
    // Size of (decompressed?) lump.
    uint32_t size;
    // Lump type.
    uint8_t type;
    // Compression.
    uint8_t cmprs;
    // Unused.
    uint16_t dummy;
    // Lump name.
    char name[16];
};


WAD::WAD WAD::load(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw std::runtime_error{"Failed to open '" + path + "'"};

    // WAD file starts with a header.
    M_Header header;
    f.read((char*)header.magic, 4);
    f.read((char*)&header.numlumps, 4);
    f.read((char*)&header.diroffset, 4);
    std::vector<M_Lump> directory{(size_t)header.numlumps};

    // Seek to the start of the directory and read the lump headers.
    f.seekg(header.diroffset);
    for (uint32_t i = 0; i < header.numlumps; ++i)
    {
        auto &entry = directory[i];
        f.read((char*)&entry.offset, 4);
        f.read((char*)&entry.dsize, 4);
        f.read((char*)&entry.size, 4);
        f.read((char*)&entry.type, 1);
        f.read((char*)&entry.cmprs, 1);
        f.read((char*)&entry.dummy, 2);
        f.read((char*)entry.name, 16);
    }

    // Copy out lump data from the WAD.
    WAD out{};
    memcpy(out.id, header.magic, 4);
    for (auto const &lump : directory)
    {
        Lump l{lump.name, lump.type, {}};
        auto dat = new uint8_t[lump.dsize];
        f.seekg(lump.offset);
        f.read((char*)dat, lump.dsize);
        l.data.insert(l.data.end(), dat, dat + lump.dsize);
        delete[] dat;
        out.directory.push_back({l});
    }

    f.close();
    return out;
}

void WAD::print(WAD const &wad)
{
    printf("ID: %.4s\n", wad.id);
    for (auto const &lump : wad.directory)
        printf("\"%s\": type=%x;data=%zd bytes\n",
            lump.name.c_str(), lump.type, lump.data.size());
}
