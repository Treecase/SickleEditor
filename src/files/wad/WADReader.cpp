/**
 * WADReader.cpp - WAD file reader.
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

#include "WADReader.hpp"


using namespace WAD;


WADReader::WADReader(WADInputStream &inputstream)
:   _stream{inputstream}
{
}


std::vector<WADReader::DirectoryEntry> const &WADReader::get_directory() const
{
    return _directory;
}


WADReader::DirectoryEntry const &WADReader::get_directory_entry(
    std::size_t index) const
{
    return _directory.at(index);
}


void WADReader::load()
{
    _header = read_header();
    _stream.seek(_header.directory_offset);
    for (uint32_t i = 0; i < _header.number_of_lumps; ++i)
    {
        _directory.push_back(read_directory_entry());
    }
}


LumpTexture WADReader::load_lump_texture(DirectoryEntry const &entry)
{
    _stream.seek(entry.lump_offset);
    unsigned char bytes[entry.dsize];
    _stream.read_bytes(bytes, entry.dsize);

    LumpTexture texlump{entry, bytes};
    return texlump;
}



WADReader::Header WADReader::read_header()
{
    Header header{};
    _stream.read_bytes(header.magic, 4);
    header.number_of_lumps = _stream.read_uint32();
    header.directory_offset = _stream.read_uint32();
    return header;
}


WADReader::DirectoryEntry WADReader::read_directory_entry()
{
    DirectoryEntry dirent{};
    dirent.lump_offset = _stream.read_uint32();
    dirent.dsize = _stream.read_uint32();
    dirent.size = _stream.read_uint32();
    dirent.type = _stream.read_uint8();
    dirent.compression = _stream.read_uint8();
    _stream.read_uint8(); // unused byte
    _stream.read_uint8(); // unused byte
    _stream.read_bytes(dirent.name, 16);
    return dirent;
}


WADReader::DirectoryEntry WADReader::read_directory_entry(size_t index)
{
    _stream.seek(_header.directory_offset + index * DIRECTORY_ENTRY_SIZE);
    return read_directory_entry();
}
