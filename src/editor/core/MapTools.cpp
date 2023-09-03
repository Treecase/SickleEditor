/**
 * MapTools.cpp - Editor MapTools.
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

#include "core/Editor.hpp"

using namespace Sickle::Editor;


MapTool::MapTool(
    std::string const &name,
    std::vector<OpDef> const &opdefs,
    decltype(x) fn)
:   _name{name}
,   _opdefs{opdefs}
,   x{fn}
{
}


std::string MapTool::name() const
{
    return _name;
}


std::vector<MapTool::OpDef> MapTool::operations() const
{
    return _opdefs;
}
