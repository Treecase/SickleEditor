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

#include "Commands.hpp"

#include <core/Editor.hpp>

#include <glm/glm.hpp>

#include <vector>

using namespace Sickle::Editor::commands;


/* ===[ AddBrush ]=== */
void AddBrush::execute(Editor &editor)
{
    auto const a = editor.brushbox.p1();
    auto const b = editor.brushbox.p2();
    editor.brushbox.p1({0, 0, 0});
    editor.brushbox.p2({0, 0, 0});

    std::vector<glm::vec3> const points{
        {a.x, a.y, a.z},
        {a.x, a.y, b.z},
        {a.x, b.y, a.z},
        {a.x, b.y, b.z},
        {b.x, a.y, a.z},
        {b.x, a.y, b.z},
        {b.x, b.y, a.z},
        {b.x, b.y, b.z},
    };
    editor.get_map()->add_brush(points);
}
