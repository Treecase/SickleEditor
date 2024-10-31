/**
 * WADReader.hpp - WAD file reader.
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

#ifndef SE_FILES_WAD_WADREADER_HPP
#define SE_FILES_WAD_WADREADER_HPP

#include "WADInputStream.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace WAD
{
    class LumpTexture;

    /**
     * Extracts data from a .wad file.
     */
    class WADReader
    {
    public:
        struct DirectoryEntry
        {
            uint32_t lump_offset;
            uint32_t dsize;
            uint32_t size;
            uint8_t type;
            uint8_t compression;
            char name[16];
        };

        WADReader(WADInputStream &inputstream);

        /**
         * Get the whole directory.
         *
         * @return A collection of all the directory entries in the WAD.
         */
        std::vector<DirectoryEntry> const &get_directory() const;

        /**
         * Get directory entry by index.
         *
         * @param index Index of the lump to load from.
         * @return The directory entry.
         */
        DirectoryEntry const &get_directory_entry(size_t index) const;

        /**
         * Load the basics from the WAD.
         */
        void load();

        /**
         * Load a texture lump.
         *
         * @param entry Directory entry for the lump.
         * @return The loaded texture lump.
         * @throw WAD::TexLumpLoadError if lump is not a texture lump.
         */
        LumpTexture load_lump_texture(DirectoryEntry const &entry);

    protected:
        static constexpr size_t HEADER_SIZE = 12;
        static constexpr size_t DIRECTORY_ENTRY_SIZE = 32;

        struct Header
        {
            char magic[4];
            uint32_t number_of_lumps;
            uint32_t directory_offset;
        };

        Header read_header();
        DirectoryEntry read_directory_entry();
        DirectoryEntry read_directory_entry(size_t index);

    private:
        Header _header;
        std::vector<DirectoryEntry> _directory{};

        WADInputStream &_stream;
    };
} // namespace WAD

#include "LumpTexture.hpp"

#endif
