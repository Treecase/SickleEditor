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

namespace std
{
    std::ostream &operator<<(std::ostream &os, MAP::Vertex const &vertex);
    std::ostream &operator<<(std::ostream &os, MAP::Plane const &plane);
    std::ostream &operator<<(std::ostream &os, MAP::Brush const &brush);
    std::ostream &operator<<(std::ostream &os, MAP::Entity const &entity);
    std::ostream &operator<<(std::ostream &os, MAP::Map const &map);
} // namespace std

namespace MAP
{
    /** Save a map to a .map file. */
    void save(std::ostream &out, Map const &map);
} // namespace MAP

#endif
