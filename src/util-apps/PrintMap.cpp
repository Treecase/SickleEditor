/**
 * PrintMap.cpp - Print .map files.
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

#include "../map/load_map.hpp"

#include <iostream>
#include <string>
#include <vector>


std::string const APP_canon_name = "showmap";
std::string const APP_version = "0.1.0";

typedef std::vector<std::string> APP_Config;


APP_Config APP_handle_args(int argc, char *argv[])
{
    APP_Config filenames{};
    for (int i = 1; i < argc; ++i)
        filenames.push_back(argv[i]);
    return filenames;
}

void APP_print_usage(char const *name)
{
    std::cout <<
        "Usage: " << name << " FILE...\n"
        "Print .map files.\n"
        "\n";
}

int APP_run(APP_Config const &filenames)
{
    for (auto const &filename : filenames)
    {
        auto map = MAP::load_map(filename);
        std::cout << "<map " << filename << ">\n";
        for (auto const &entity : map.entities)
        {
            std::cout << "  {\n";
            for (auto const &property : entity.properties)
                std::cout << "    " << property.first << ": " << property.second << "\n";
            for (auto const &brush : entity.brushes)
            {
                std::cout << "    {\n";
                for (auto const &plane : brush.planes)
                {
                    std::cout <<
                        "      (" << plane.a[0] << " " << plane.a[1] << " " << plane.a[2] << ")"
                        << " (" << plane.b[0] << " " << plane.b[1] << " " << plane.b[2] << ")"
                        << " (" << plane.c[0] << " " << plane.c[1] << " " << plane.c[2] << ")"
                        << " " << plane.miptex
                        << " [" << plane.offx[0] << ", " << plane.offx[1] << ", " << plane.offx[2] << ", " << plane.offx[3] << "]"
                        << " [" << plane.offy[0] << ", " << plane.offy[1] << ", " << plane.offy[2] << ", " << plane.offy[3] << "]"
                        << " "  << plane.rotation
                        << " "  << plane.scalex << " " << plane.scaley
                        << "\n";
                }
                std::cout << "    }\n";
            }
            std::cout << "  }\n";
        }
        std::cout << "</map " << filename << ">\n";
    }
    return EXIT_SUCCESS;
}

#include "Generic-main.cpp"
