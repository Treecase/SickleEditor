/**
 * mapsaver.cpp - Save a map to a .map file.
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

#include "map/mapsaver.hpp"

#include <iomanip>
#include <ostream>


namespace std
{
std::ostream &operator<<(std::ostream &os, MAP::Vertex const &vertex)
{
    return os << "( " << vertex.x << ' ' << vertex.y << ' ' << vertex.z << " )";
}


std::ostream &operator<<(std::ostream &os, MAP::Plane const &plane)
{
    os << plane.a << ' ' << plane.b << ' ' << plane.c
        << ' ' << plane.miptex
        << " [ " << plane.s.x << ' ' << plane.s.y << ' ' << plane.s.z
        <<  ' ' << plane.offsets.x << " ] "
        << "[ "<< plane.t.x << ' ' << plane.t.y << ' ' << plane.t.z
        << ' ' << plane.offsets.y << " ] "
        << plane.rotation
        << ' ' << plane.scale.x << ' ' << plane.scale.y;
    return os;
}


std::ostream &operator<<(std::ostream &os, MAP::Brush const &brush)
{
    os << "{\n";
    for (auto const &plane : brush.planes)
        os << plane << '\n';
    os << '}';
    return os;
}


std::ostream &operator<<(std::ostream &os, MAP::Entity const &entity)
{
    os << "{\n";
    os << "\"classname\" \"" << entity.properties.at("classname") << "\"\n";
    for (auto const &kv : entity.properties)
        if (kv.first != "classname")
            os << '"' << kv.first << "\" \"" << kv.second << "\"\n";
    for (auto const &brush : entity.brushes)
        os << brush << '\n';
    os << '}';
    return os;
}


std::ostream &operator<<(std::ostream &os, MAP::Map const &map)
{
    for (auto const &ent : map.entities)
        os << ent << '\n';
    return os;
}
}


void MAP::save(std::ostream &out, Map const &map)
{
    // TODO: Using std::fixed here so we don't get output in scientific
    // notation. Downside is it makes the output look ugly with all the extra
    // 0s. Would prefer to find a better way to do this.
    out << std::fixed << map;
}
