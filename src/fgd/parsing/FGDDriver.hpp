/**
 * FGDDriver.hpp - Flex/Bison .fgd parser driver.
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

#ifndef FGD_DRIVER_HPP
#define FGD_DRIVER_HPP

#include "FGDParser.hpp"
#include "FGDScanner.hpp"
#include "fgd/fgd.hpp"

#include <istream>
#include <memory>


namespace FGD
{
    class FGDDriver
    {
    public:
        void set_debug(bool debug);
        void parse(std::istream &iss);
        GameDef get_result() const;
        GameDef result;

    protected:
        bool debug_enabled;
        std::unique_ptr<FGDParser> _parser;
        std::unique_ptr<FGDScanner> _scanner;
    };
}

#endif /**/
