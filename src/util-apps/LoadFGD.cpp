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

#include "../fgd/fgd.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


std::string const APP_canon_name = "loadfgd";
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
        "Print .fgd files.\n"
        "\n";
}

int APP_run(APP_Config const &filenames)
{
    for (auto const &filename : filenames)
    {
        auto fgd = FGD::load(filename);
        std::cout << "<fgd " << filename << ">\n";
        for (auto const &cls : fgd)
        {
            std::cout << '@' << cls.type;
            for (auto const &property : cls.properties)
            {
                std::cout << ' ' << property.name << '(';
                if (property.name == "size")
                {
                    std::cout << property.arguments[0];
                    for (size_t i = 1; i < property.arguments.size(); ++i)
                    {
                        if (i % 3 == 0)
                            std::cout << ',';
                        std::cout << ' ' << property.arguments[i];
                    }
                }
                else if (property.name == "base")
                {
                    std::cout << property.arguments[0];
                    for (size_t i = 1; i < property.arguments.size(); ++i)
                        std::cout << ", " << property.arguments[i];
                }
                else if (property.name == "iconsprite" || property.name == "studio")
                {
                    std::cout << '"' << property.arguments[0] << '"';
                }
                else
                    for (auto const &argument : property.arguments)
                        std::cout << ' ' << argument;
                std::cout << ')';
            }
            std::cout << " = " << cls.name;
            if (!cls.description.empty())
                std::cout << " : \"" << cls.description << '"';
            std::cout << "\n[\n";
            for (auto const &field : cls.fields)
            {
                std::cout << "  " << field.name << '(' << field.type << ')';
                if (!field.description.empty())
                    std::cout << " : \"" << field.description << '"';
                if (!field.defaultvalue.empty())
                    std::cout << " : \"" << field.defaultvalue << '"';
                if (field.type == "choices")
                {
                    std::cout << " =\n  [\n";
                    for (auto const &choice : field.choices)
                        std::cout << "    " << choice.value << " : \"" << choice.description << "\"\n";
                    std::cout << "  ]\n";
                }
                else if (field.type == "flags")
                {
                    std::cout << " =\n  [\n";
                    for (auto const &flag : field.flags)
                        std::cout << "    " << flag.value << " : \"" << flag.description << "\" : " << flag.init << '\n';
                    std::cout << "  ]\n";
                }
                std::cout << '\n';
            }
            std::cout << "]\n";
        }
        std::cout << "</fgd " << filename << ">\n";
    }
    return EXIT_SUCCESS;
}

#include "Generic-main.cpp"
