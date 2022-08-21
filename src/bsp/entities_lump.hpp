/**
 * entities_lump.hpp - Parse the .bsp Entities lump.
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

#ifndef _ENTITIES_LUMP_HPP
#define _ENTITIES_LUMP_HPP

#include <string>
#include <unordered_map>
#include <vector>


typedef std::unordered_map<std::string, std::string> Entity;


/** Parse a .bsp's Entities lump. */
std::vector<Entity> parse_entities(std::string const &buf);

#endif
