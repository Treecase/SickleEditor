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


sigc::signal<void(std::string)> World3D::Face::_signal_missing_texture{};


World3D::Face::Face(
    Sickle::Editor::FaceRef face,
    TexManRef texman,
    GLint offset)
:   sigc::trackable{}
,   _src{face}
,   _texman{texman}
,   offset{offset}
{
    _on_src_texture_changed();

    _sync_vertices();

    _src->signal_vertices_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_verts_changed));
    _src->property_texture().signal_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_texture_changed));
}


World3D::Face::Face(Face const &other)
:   sigc::trackable{}
,   _src{other._src}
,   _texman{other._texman}
,   texture{other.texture}
,   vertices{other.vertices}
,   offset{other.offset}
{
    _src->signal_vertices_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_verts_changed));
    _src->property_texture().signal_changed().connect(
        sigc::mem_fun(*this, &Face::_on_src_texture_changed));
}


void World3D::Face::_on_src_verts_changed()
{
    vertices.clear();
    _sync_vertices();
    signal_verts_changed().emit();
}


void World3D::Face::_on_src_texture_changed()
{
    printf("texture changed to '%s'\n", _src->get_texture().c_str());
    try {
        texture = _texman->at(_src->get_texture());
    }
    catch (std::out_of_range const &e) {
        texture = Texture::make_missing_texture();
        signal_missing_texture().emit(_src->get_texture());
    }
}


void World3D::Face::_sync_vertices()
{
    auto const &u_axis = glm::normalize(_src->get_u());
    auto const &v_axis = glm::normalize(_src->get_v());
    glm::vec2 const texture_size{texture.width, texture.height};

    for (auto const &vertex : _src->get_vertices())
    {
        vertices.emplace_back(
            vertex,
            (   (   glm::vec2{
                        glm::dot(vertex, u_axis),
                        glm::dot(vertex, v_axis)}
                    / _src->get_scale())
                + _src->get_shift())
            / texture_size);
    }
}
