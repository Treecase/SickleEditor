/**
 * Commands.cpp - Editor Command classes.
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

#include "editor/Commands.hpp"
#include "editor/Editor.hpp"

#include <convexhull/convexhull.hpp>

#include <cstdio>


using namespace Sickle::Editor::commands;


/* ===[ AddBrush ]=== */
AddBrush::AddBrush(std::vector<glm::vec3> const &points)
:   _points{points}
{
}


void AddBrush::execute(Editor &editor)
{
    auto brush = std::make_shared<Brush>(_points);
    auto &map = editor.get_map();
    for (auto &entity : map.entities)
        if (entity.properties.at("classname") == "worldspawn")
            entity.brushes.push_back(brush);
}
