/**
 * Brush.cpp - Editor::Brush.
 * Copyright (C) 2023-2024 Trevor Last
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

#include "Brush.hpp"

#include <config/appid.hpp>

#include <sstream>

using namespace Sickle::Editor;

BrushRef Brush::create()
{
    return Glib::RefPtr{new Brush()};
}

BrushRef Brush::create(std::vector<glm::vec3> const &vertices)
{
    auto const v = facet_enumeration(vertices);
    auto const facets = v.first;
    auto const vertices2 = v.second;

    auto result = Brush::create();
    for (auto const &facet : facets)
    {
        auto const face = Face::create(facet, vertices2);
        result->_faces.push_back(face);
        result->signal_child_added().emit(face);
    }
    return result;
}

BrushRef Brush::create(MAP::Brush const &brush)
{
    std::vector<HalfPlane> halfplanes{};
    for (auto const &plane : brush.planes)
    {
        halfplanes.emplace_back(plane.a, plane.b, plane.c);
    }
    auto const vertices = vertex_enumeration(halfplanes);
    assert(vertices.size() != 0);

    auto result = Brush::create();
    for (auto const &plane : brush.planes)
    {
        auto const face = Face::create(plane, vertices);
        result->_faces.push_back(face);
        result->signal_child_added().emit(face);
    }
    return result;
}

BrushRef Brush::create(RMF::Solid const &solid)
{
    auto result = Brush::create();
    for (auto const &map_face : solid.faces)
    {
        auto face = Face::create(map_face);
        result->_faces.push_back(face);
        result->signal_child_added().emit(face);
    }
    return result;
}

Brush::Brush()
: Glib::ObjectBase{typeid(Brush)}
, Lua::Referenceable{}
{
    signal_removed().connect(sigc::mem_fun(*this, &Brush::on_removed));
}

Brush::~Brush()
{
    for (auto const &face : _faces)
    {
        signal_child_removed().emit(face);
    }
}

Brush::operator MAP::Brush() const
{
    MAP::Brush out{};
    for (auto const &face : _faces)
    {
        out.planes.push_back(*face.get());
    }
    return out;
}

std::vector<FaceRef> Brush::faces() const
{
    return _faces;
}

void Brush::transform(glm::mat4 const &matrix)
{
    for (auto &face : _faces)
    {
        for (size_t i = 0; i < face->get_vertices().size(); ++i)
        {
            face->set_vertex(
                i,
                glm::vec3{
                    matrix * glm::vec4{face->get_vertex(i), 1.0}
            });
        }
    }
}

void Brush::translate(glm::vec3 const &translation)
{
    transform(glm::translate(glm::mat4{1.0}, translation));
}

/* ---[ EditorObject interface ]--- */
Glib::ustring Brush::name() const
{
    std::stringstream ss{};
    ss << "Brush " << this;
    return Glib::ustring{ss.str()};
}

Glib::RefPtr<Gdk::Pixbuf> Brush::icon() const
{
    return Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX
                                             "icons/outliner/brush.png");
}

std::vector<EditorObjectRef> Brush::children() const
{
    std::vector<EditorObjectRef> out{};
    for (auto const &face : _faces)
    {
        out.push_back(face);
    }
    return out;
}

void Brush::on_removed()
{
    select(false);
}
