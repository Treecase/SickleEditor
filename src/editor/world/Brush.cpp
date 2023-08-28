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

#include "world/Brush.hpp"

using namespace Sickle::Editor;


Brush::Brush()
:   Glib::ObjectBase{typeid(Brush)}
,   Lua::Referenceable{}
,   _prop_selected{*this, "selected", false}
{
}


Brush::Brush(Brush const &other)
:   Brush{}
{
    faces = other.faces;
    property_selected().set_value(other.is_selected());
}


Brush::Brush(std::vector<glm::vec3> const &vertices)
:   Brush{}
{
    auto const v = facet_enumeration(vertices);
    auto const facets = v.first;
    auto const vertices2 = v.second;
    for (auto const &facet : facets)
        faces.emplace_back(std::make_shared<Face>(facet, vertices2));
}


Brush::Brush(MAP::Brush const &brush)
:   Brush{}
{
    std::vector<HalfPlane> halfplanes{};
    for (auto const &plane : brush.planes)
        halfplanes.emplace_back(plane.a, plane.b, plane.c);
    auto const vertices = vertex_enumeration(halfplanes);
    assert(vertices.size() != 0);
    for (auto const &plane : brush.planes)
        faces.emplace_back(std::make_shared<Face>(plane, vertices));
}


Brush::Brush(RMF::Solid const &solid)
:   Brush{}
{
    for (auto const &face : solid.faces)
        faces.emplace_back(std::make_shared<Face>(face));
}


Brush::operator MAP::Brush() const
{
    MAP::Brush out{};
    for (auto const &face : faces)
        out.planes.push_back(*face);
    return out;
}


void Brush::transform(glm::mat4 const &matrix)
{
    for (auto &face : faces)
        for (size_t i = 0; i < face->vertices.size(); ++i)
            face->set_vertex(
                i, glm::vec3{matrix * glm::vec4{face->vertices[i], 1.0}});
}


void Brush::translate(glm::vec3 const &translation)
{
    transform(glm::translate(glm::mat4{1.0}, translation));
}
