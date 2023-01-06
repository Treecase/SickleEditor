/**
 * LoadFGD.cpp - Print .fgd files.
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

#include "fgd/fgd.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


std::string const APP_canon_name = "loadfgd";
std::string const APP_version = "1.0.0";

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
        "Print .fgd files.\n"
        "\n";
}

int APP_run(APP_Config const &filenames)
{
    for (auto const &filename : filenames)
    {
        std::cout << "<fgd " << filename << ">\n";
        std::cout << FGD::from_file(filename) << '\n';
        std::cout << "</fgd " << filename << ">\n";
    }
    return EXIT_SUCCESS;
}

#include "Generic-main.cpp"
