/**
 * LumpTexture.hpp - WAD3 texture lump.
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

#ifndef SE_FILES_WAD_LUMPTEXTURE_HPP
#define SE_FILES_WAD_LUMPTEXTURE_HPP

#include "WADReader.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace WAD
{
    /**
     * Texture lump. Type 0x43.
     */
    class LumpTexture
    {
    public:
        /**
         * Thrown if constructor recieves an entry with an invalid type field.
         */
        struct BadTypeException : std::runtime_error
        {
            BadTypeException(std::string const &what)
            : std::runtime_error{what}
            {
            }
        };

        LumpTexture() = default;

        /** Name of the lump in the directory. */
        std::string name() const;

        /** Internal name of the texture. */
        std::string texture_name() const;

        /** Width of full size texture in pixels. */
        uint32_t width() const;

        /** Height of full size texture in pixels. */
        uint32_t height() const;

        /** Pixels making up the full size texture. */
        std::vector<uint8_t> const &tex1() const;

        /** Pixels making up the half size texture. */
        std::vector<uint8_t> const &tex2() const;

        /** Pixels making up the quarter size texture. */
        std::vector<uint8_t> const &tex4() const;

        /** Pixels making up the eighth size texture. */
        std::vector<uint8_t> const &tex8() const;

        /** RGB triples comprising the palette. */
        std::vector<std::array<uint8_t, 3>> const &palette() const;

    protected:
        friend class WADReader;

        LumpTexture(
            WADReader::DirectoryEntry const &entry,
            uint8_t const *bytes);

    private:
        std::string _name{};
        std::string _texture_name{};
        uint32_t _width, _height;

        std::array<std::vector<uint8_t>, 4> _textures{};
        std::vector<std::array<uint8_t, 3>> _palette{};
    };
} // namespace WAD

#endif
