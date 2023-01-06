/**
 * FGDDriver.cpp - Flex/Bison .fgd parser driver.
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

#include "FGDDriver.hpp"

#include <stdexcept>
#include <fstream>


void FGD::FGDDriver::set_debug(bool debug)
{
    debug_enabled = debug;
}

void FGD::FGDDriver::parse(std::istream &iss)
{
    _scanner.reset(new FGD::FGDScanner{&iss});
    _parser.reset(new FGD::FGDParser{*_scanner, *this});

    std::ofstream dbgstream{};
    if (debug_enabled)
    {
        dbgstream.open("debug.txt");
        _parser->set_debug_level(1);
        _parser->set_debug_stream(dbgstream);
    }

    if (_parser->parse() != 0)
        throw std::runtime_error{"parse failed"};
}

FGD::GameDef FGD::FGDDriver::get_result() const
{
    return result;
}
