/**
 * Face.cpp - World3D::Face class.
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

#include "world3d/Face.hpp"

#include <wad/TextureManager.hpp>
#include <glm/gtx/rotate_vector.hpp>


sigc::signal<void(std::string)> World3D::Face::_signal_missing_texture{};


World3D::Face::Face(Sickle::Editor::FaceRef const &face, GLint offset)
:   sigc::trackable{}
,   _src{face}
,   _offset{offset}
{
    _src->property_texture().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_texture_changed));

    _src->property_u().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_uv_changed));
    _src->property_v().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_uv_changed));

    _src->property_shift().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_shift_changed));

    _src->property_scale().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_scale_changed));

    _src->property_rotation().signal_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_rotation_changed));

    _src->signal_vertices_changed().connect(
        sigc::mem_fun(*this, &Face::on_src_verts_changed));

    push_queue([this](){_sync_texture();});
    _sync_vertices();
}



void World3D::Face::on_src_texture_changed()
{
    push_queue([this](){_sync_texture();});
}


void World3D::Face::on_src_uv_changed()
{
    _sync_vertices();
}


void World3D::Face::on_src_shift_changed()
{
    _sync_vertices();
}


void World3D::Face::on_src_scale_changed()
{
    _sync_vertices();
}


void World3D::Face::on_src_rotation_changed()
{
    _sync_vertices();
}


void World3D::Face::on_src_verts_changed()
{
    _sync_vertices();
}



void World3D::Face::_sync_vertices()
{
    auto const u_base = glm::normalize(_src->get_u());
    auto const v_base = glm::normalize(_src->get_v());
    auto const normal = glm::normalize(glm::cross(u_base, v_base));

    // TODO: This doesn't seem quite right? idk
    auto const u_axis = glm::rotate(
        u_base,
        glm::radians(_src->get_rotation()),
        normal);
    auto const v_axis = glm::rotate(
        v_base,
        glm::radians(_src->get_rotation()),
        normal);

    glm::vec2 const texture_size{_texture.width, _texture.height};

    _vertices.clear();
    for (auto const &vertex : _src->get_vertices())
    {
        glm::vec2 uv{
            glm::dot(vertex, u_axis),
            glm::dot(vertex, v_axis)};

        uv = ((uv / _src->get_scale()) + _src->get_shift()) / texture_size;

        _vertices.emplace_back(vertex, uv);
    }
    signal_vertices_changed().emit();
}


void World3D::Face::_sync_texture()
{
    auto &texman = WAD::TextureManagerProxy<Texture>::create();
    try {
        _texture = texman.at(_src->get_texture());
    }
    catch (std::out_of_range const &e) {
        _texture = Texture::make_missing_texture();
        signal_missing_texture().emit(_src->get_texture());
    }
    _sync_vertices();
}
