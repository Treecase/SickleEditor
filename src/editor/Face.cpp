/**
 * Face.cpp - Editor::Face.
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

#include "editor/EditorWorld.hpp"


Sickle::Editor::Face::Face(
    std::vector<glm::vec3> vertices,
    std::string texture,
    glm::vec3 u,
    glm::vec3 v,
    glm::vec2 shift,
    glm::vec2 scale,
    float rotation)
:   vertices{vertices}
,   texture{texture}
,   u{u}
,   v{v}
,   shift{shift}
,   scale{scale}
,   rotation{rotation}
{
    if (vertices.size() < 3)
        throw std::runtime_error{"not enough points for a face"};
}


Sickle::Editor::Face::operator MAP::Plane() const
{
    auto abc = get_plane_points();
    MAP::Plane out{
        abc[0], abc[1], abc[2],
        vertices,
        texture,
        u, v,
        shift,
        rotation,
        scale
    };
    return out;
}


std::array<glm::vec3, 3> Sickle::Editor::Face::get_plane_points() const
{
    return {vertices.at(0), vertices.at(1), vertices.at(2)};
}


void Sickle::Editor::Face::set_vertex(size_t index, glm::vec3 vertex)
{
    vertices.at(index) = vertex;
    signal_vertices_changed().emit();
}
