/**
 * MAPDriver.hpp - Flex/Bison .map parser driver.
 * Copyright (C) 2023 Trevor Last
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

#ifndef MAP_DRIVER_HPP
#define MAP_DRIVER_HPP

#include "MAPParser.hpp"
#include "MAPScanner.hpp"
#include "map/map.hpp"

#include <istream>
#include <memory>


namespace MAP
{
    class MAPDriver
    {
    public:
        void set_debug(bool debug);
        void parse(std::istream &iss);
        Map get_result() const;
        Map result{};

    protected:
        bool debug_enabled{false};
        std::unique_ptr<MAPParser> _parser;
        std::unique_ptr<MAPScanner> _scanner;
    };
}

#endif
