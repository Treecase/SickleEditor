/**
 * load_map.hpp - Load .map files.
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

#ifndef _LOAD_MAP_HPP
#define _LOAD_MAP_HPP

#include <array>
#include <string>
#include <unordered_map>
#include <vector>


namespace MAP
{
    struct Plane
    {
        std::array<float, 3> a, b, c;
        std::string miptex;
        std::array<float, 4> offx;
        std::array<float, 4> offy;
        float rotation;
        float scalex, scaley;
    };
    struct Brush
    {
        std::vector<Plane> planes;
    };
    struct Entity
    {
        std::unordered_map<std::string, std::string> properties;
        std::vector<Brush> brushes;
    };
    struct Map
    {
        std::vector<Entity> entities;
    };

    /** Parse a .map file. */
    Map load_map(std::string const &path);
}

#endif
