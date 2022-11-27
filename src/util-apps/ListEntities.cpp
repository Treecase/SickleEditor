/**
 * ListEntities.cpp - List entries in a .bsp's Entities lump.
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

#include "../bsp/load_bsp.hpp"

#include <iostream>
#include <string>
#include <vector>


std::string const APP_canon_name = "list-entities";
std::string const APP_version = "0.1.0";

typedef std::vector<std::string> APP_Config;


APP_Config APP_handle_args(int argc, char *argv[])
{
    std::vector<std::string> filenames{};
    for (int i = 1; i < argc; ++i)
        filenames.push_back(argv[i]);
    return filenames;
}

void APP_print_usage(char const *name)
{
    std::cout <<
        "Usage: " << name << " FILE...\n"
        "Lists Entities from a .bsp Entities lump.\n"
        "\n";
}

int APP_run(APP_Config const &filenames)
{
    for (auto const &filename : filenames)
    {
        auto bsp = BSP::load(filename);
        std::cout << "<" << filename << ">\n";
        for (auto const &entity : bsp.entities)
        {
            std::cout << "  {\n";
            for (auto const &property : entity)
            {
                std::cout << "    " << property.first << ": " << property.second << "\n";
            }
            std::cout << "  }\n";
        }
        std::cout << "</" << filename << ">\n";
    }
    return EXIT_SUCCESS;
}

#include "Generic-main.cpp"
