/**
 * fgd.cpp - Load .fgd files.
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

#include "fgd.hpp"
#include "parsing/FGDDriver.hpp"

#include <iostream>
#include <fstream>


FGD::GameDef FGD::from_file(std::string const &path)
{
    FGD::FGDDriver driver{};
    std::ifstream in{path};
    driver.parse(in);
    return driver.get_result();
}

std::ostream &FGD::operator<<(std::ostream &os, GameDef const &fgd)
{
    for (auto const &cls : fgd.classes)
        os << *cls << '\n';
    return os;
}

std::ostream &FGD::operator<<(std::ostream &os, Class const &cls)
{
    os << '@';
    std::cout << cls.type();
    for (auto const &attr : cls.attributes)
        os << ' ' << *attr;
    os << " = " << cls.name;
    if (cls.description.has_value())
        os << " : \"" << cls.description.value() << '"';
    os << (cls.properties.empty()? ' ' : '\n') << '[';
    for (auto const &prop : cls.properties)
        os << "\n\t" << *prop;
    os << (cls.properties.empty()? "" : "\n") << ']';
    return os;
}

std::ostream &FGD::operator<<(std::ostream &os, Attribute const &attr)
{
    return os << attr.name() << '(' << attr.inner() << ')';
}

std::ostream &FGD::operator<<(std::ostream &os, Property const &prop)
{
    return os << prop.name << '(' << prop.type() << ')' << prop.rest();
}
