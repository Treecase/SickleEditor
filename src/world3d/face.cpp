/**
 * face.cpp - World3D::Face class.
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

#include "world3d/world3d.hpp"


World3D::Face::Face(
    Brush &parent, std::shared_ptr<Sickle::Editor::Face> &face)
:   _parent{parent}
,   _src{face}
{
    try
    {
        texture = texman().at(face->texture);
    }
    catch (std::out_of_range const &e)
    {
        // TODO: Emit a "missing textures" signal?
        texture = Texture::make_missing_texture();
    }

    _sync_vertices();

    _verts_changed_connection = _src->signal_vertices_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_verts_changed));
}


World3D::Face::Face(Face const &other)
:   _parent{other._parent}
,   _src{other._src}
,   texture{other.texture}
,   vertices{other.vertices}
,   offset{other.offset}
{
    _verts_changed_connection = _src->signal_vertices_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_verts_changed));
}


World3D::Face::~Face()
{
    _verts_changed_connection.disconnect();
}


World3D::TextureManager &World3D::Face::texman() const
{
    return _parent.texman();
}


void World3D::Face::_on_src_verts_changed()
{
    vertices.clear();
    _sync_vertices();
    signal_verts_changed().emit();
}


void World3D::Face::_sync_vertices()
{
    auto const &u_axis = glm::normalize(_src->u.get());
    auto const &v_axis = glm::normalize(_src->v.get());
    glm::vec2 const texture_size{texture.width, texture.height};

    for (auto const &vertex : _src->vertices)
    {
        vertices.emplace_back(
            vertex,
            (   (   glm::vec2{
                        glm::dot(vertex, u_axis),
                        glm::dot(vertex, v_axis)}
                    / _src->scale.get())
                + _src->shift.get())
            / texture_size);
    }
}
