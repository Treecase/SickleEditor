/**
 * load_bsp.hpp - Load .bsp files.
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

#ifndef _LOAD_BSP
#define _LOAD_BSP

#include <memory>
#include <string>
#include <vector>


namespace BSP
{
    struct Texture
    {
        std::string name;
        size_t width, height;
        std::shared_ptr<uint8_t> tex1; // full size mipmap
        std::shared_ptr<uint8_t> tex2; // half size mipmap
        std::shared_ptr<uint8_t> tex4; // quarter size mipmap
        std::shared_ptr<uint8_t> tex8; // eighth size mipmap
    };

    /** Loaded .bsp data. */
    struct BSP
    {
        std::vector<Texture> textures;
    };

    /** Load a .bsp file. */
    BSP load_bsp(std::string const &path);
}

#endif
