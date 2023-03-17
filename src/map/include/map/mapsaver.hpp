/**
 * mapsaver.hpp - Save a map to a .map file.
 * Copyright (C) 2023 Trevor Last
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

#ifndef SE_MAPSAVER_HPP
#define SE_MAPSAVER_HPP

#include "map.hpp"

#include <ostream>


namespace MAP
{
    /** Save a map to a .map file. */
    void save(std::ostream &out, Map const &map)
    {
        for (auto const &entity : map.entities)
        {
            out << "{\n";
            out << "\"classname\" \"" << entity.properties.at("classname") << "\"\n";
            for (auto const &property : entity.properties)
            {
                if (property.first != "classname")
                    out << "\"" << property.first << "\" \"" << property.second
                        << "\"\n";
            }
            for (auto const &brush : entity.brushes)
            {
                out << "{\n";
                for (auto const &plane : brush.planes)
                {
                    out
                        << "( "
                        << plane.a.x << ' ' << plane.a.y << ' ' << plane.a.z
                        << " ) ( "
                        << plane.b.x << ' ' << plane.b.y << ' ' << plane.b.z
                        << " ) ( "
                        << plane.c.x << ' ' << plane.c.y << ' ' << plane.c.z
                        << " ) "
                        << plane.miptex
                        << " [ "
                        << plane.s.x << ' ' << plane.s.y << ' ' << plane.s.z
                        << ' ' << plane.offsets.s
                        << " ] [ "
                        << plane.t.x << ' ' << plane.t.y << ' ' << plane.t.z
                        << ' ' << plane.offsets.t
                        << " ] " << plane.rotation
                        << ' ' << plane.scale.x << ' ' << plane.scale.y
                        << '\n';
                }
                out << "}\n";
            }
            out << "}\n";
        }
    }
}

#endif
