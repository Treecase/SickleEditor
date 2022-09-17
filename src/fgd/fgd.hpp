/**
 * fgd.hpp - Load .fgd files.
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

#ifndef _FGD_HPP
#define _FGD_HPP

#include <string>
#include <vector>


namespace FGD
{
    struct Flag
    {
        unsigned long value;
        std::string description;
        bool init;
    };

    struct Choice
    {
        unsigned long value;
        std::string description;
    };

    struct Field
    {
        std::string name;
        std::string type;
        std::string description;
        std::string defaultvalue;
        // Only used if type is "choices"
        std::vector<Choice> choices;
        // Only used if type is "flags"
        std::vector<Flag> flags;
    };

    struct Property
    {
        std::string name;
        std::vector<std::string> arguments;
    };

    struct Class
    {
        std::string type;
        std::vector<Property> properties;
        std::string name;
        std::string description;
        std::vector<Field> fields;
    };

    typedef std::vector<Class> FGD;

    /** Parse a .map file. */
    FGD load(std::string const &path);
}

#endif
