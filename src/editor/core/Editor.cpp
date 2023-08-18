/**
 * Editor.cpp - Editor::Editor.
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
#include <commands/Commands.hpp>

using namespace Sickle::Editor;


Editor::Editor()
{
    signal_map_changed().connect(
        sigc::mem_fun(*this, &Editor::_on_map_changed));
}



void Editor::set_map(Map const &map)
{
    _map = map;
    signal_map_changed().emit();
}


void Editor::do_command(std::shared_ptr<Command> command)
{
    command->execute(*this);
}



Map &Editor::get_map()
{
    return _map;
}



void Editor::_on_map_changed()
{
    brushbox = BrushBox{};
    selected.clear();
}
