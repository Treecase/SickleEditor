/**
 * lumps.hpp - WAD lump types.
 * Copyright (C) 2022-2024 Trevor Last
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
#include <optional>
#include <stdexcept>
#include <vector>
#include <memory>

#include <cstdint>


namespace WAD
{
    /** Thrown when loading a TexLump fails. */
    struct TexLumpLoadError : public std::runtime_error
    {
        std::string const name;
        TexLumpLoadError(Lump const &lump, std::string const &what);
    };

    /**
     * Texture lump. Type 0x43.
     */
    struct TexLump
    {
        std::string name() const;
        std::string texture_name() const;
        uint32_t width() const;
        uint32_t height() const;
        std::vector<uint8_t> tex1() const;
        std::vector<uint8_t> tex2() const;
        std::vector<uint8_t> tex4() const;
        std::vector<uint8_t> tex8() const;
        std::vector<std::array<uint8_t, 3>> palette() const;

        TexLump()=default;
        TexLump(TexLump const &)=default;
        TexLump(Lump const &src);

    private:
        struct DataCache
        {
            std::optional<std::vector<uint8_t>> tex1{};
            std::optional<std::vector<uint8_t>> tex2{};
            std::optional<std::vector<uint8_t>> tex4{};
            std::optional<std::vector<uint8_t>> tex8{};
            std::optional<std::vector<std::array<uint8_t, 3>>> palette{};
        };

        std::shared_ptr<Lump> _src{nullptr};
        std::shared_ptr<DataCache> _cached{nullptr};

        std::string _name{};
        std::string _texture_name{};
        uint32_t _width, _height;
        std::array<uint32_t, 4> _ptrs{};
    };

    /* Read a TexLump from a Lump. */
    TexLump readTexLump(Lump const &lump);
}

#endif
