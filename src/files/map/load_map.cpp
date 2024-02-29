/**
 * load_map.cpp - Load .map files.
 * Copyright (C) 2022-2023 Trevor Last
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

#include "map.hpp"
#include "parsing/MAPDriver.hpp"

#include <fstream>


MAP::Map MAP::load(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw MAP::LoadError{"Failed to open '" + path + "'"};
    MAPDriver driver{};
    driver.parse(f);
    return driver.get_result();
}
