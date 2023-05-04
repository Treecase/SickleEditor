/**
 * Brush.cpp - Editor::Brush.
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


Sickle::Editor::Brush::Brush(MAP::Brush const &brush)
{
    for (auto const &plane : brush.planes)
    {
        faces.emplace_back(
            std::make_shared<Face>(
                plane.vertices,
                plane.miptex,
                plane.s, plane.t,
                plane.offsets,
                plane.scale,
                plane.rotation));
    }
}


Sickle::Editor::Brush::Brush(RMF::Solid const &solid)
{
    for (auto const &face : solid.faces)
    {
        std::vector<glm::vec3> verts{};
        for (auto const &vert : face.vertices)
            verts.emplace(verts.begin(), vert.x, vert.y, vert.z);
        faces.emplace_back(
            std::make_shared<Face>(
                verts,
                face.texture_name,
                glm::vec3{
                    face.texture_u.x,
                    face.texture_u.y,
                    face.texture_u.z},
                glm::vec3{
                    face.texture_v.x,
                    face.texture_v.y,
                    face.texture_v.z},
                glm::vec2{face.texture_x_shift, face.texture_y_shift},
                glm::vec2{face.texture_x_scale, face.texture_y_scale},
                face.texture_rotation));
    }
}


Sickle::Editor::Brush::operator MAP::Brush() const
{
    MAP::Brush out{};
    for (auto const &face : faces)
        out.planes.push_back(*face);
    return out;
}


void Sickle::Editor::Brush::transform(glm::mat4 const &matrix)
{
    for (auto &face : faces)
        for (size_t i = 0; i < face->vertices.size(); ++i)
            face->set_vertex(
                i, glm::vec3{matrix * glm::vec4{face->vertices[i], 1.0}});
}


void Sickle::Editor::Brush::translate(glm::vec3 const &translation)
{
    transform(glm::translate(glm::mat4{1.0}, translation));
}
