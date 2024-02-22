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
#include "world3d/Brush.hpp"

#include <wad/TextureManager.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <numeric>


World3D::Face::SlotPreDraw World3D::Face::predraw = [](auto, auto){};
sigc::signal<void(std::string)> World3D::Face::_signal_missing_texture{};


GLUtil::Program &World3D::Face::shader()
{
    return Brush::shader();
}



World3D::Face::Face(Sickle::Editor::FaceRef const &face, GLint offset)
:   _src{face}
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

    _src->signal_removed().connect(
        sigc::mem_fun(_src, &Sickle::Editor::FaceRef::reset));

    push_queue([this](){_sync_texture();});
    _sync_vertices();
}


void World3D::Face::render() const
{
    if (!_src)
        return;

    // Modulate to "selected" color if the face or any of its parents is
    // selected.
    // TODO: May want to do this better, since we'll be walking up and down the
    // tree a bunch when we could just set a flag on the way down instead.
    glm::vec3 modulate{1.0f, 1.0f, 1.0f};
    for (
        Sickle::Editor::EditorObject const *obj = _src.get();
        obj;
        obj = obj->parent())
    {
        if (obj->is_selected())
            modulate = glm::vec3{1.0f, 0.0f, 0.0f};
    }
    shader().setUniformS("modulate", modulate);

    texture().texture->bind();
    predraw(Brush::shader(), _src.get());
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
    if (!_src)
        return;

    auto const u_axis = glm::normalize(_src->get_u());
    auto const v_axis = glm::normalize(_src->get_v());
    auto const normal = glm::normalize(glm::cross(u_axis, v_axis));

    // Find center of the face.
    auto const vertices = _src->get_vertices();
    auto const center = (
        std::accumulate(vertices.cbegin(), vertices.cend(), glm::vec3{0.0})
        / (float)vertices.size());
    // Convert center to UV coordinates.
    glm::vec2 const uv_origin{
        glm::dot(center, u_axis),
        glm::dot(center, v_axis)};

    glm::vec2 const texture_size{_texture.width, _texture.height};

    _vertices.clear();
    for (auto const &vertex : _src->get_vertices())
    {
        glm::vec2 uv{
            glm::dot(vertex, u_axis),
            glm::dot(vertex, v_axis)};

        uv -= uv_origin;
        // Rotate about center of face.
        uv = glm::rotate(uv, -glm::radians(_src->get_rotation()));
        // Scale UV from center of face.
        uv /= _src->get_scale();
        uv += uv_origin;
        // Shift UV.
        uv += _src->get_shift();
        // Normalize.
        uv /= texture_size;

        _vertices.emplace_back(vertex, uv);
    }
    signal_vertices_changed().emit();
}


void World3D::Face::_sync_texture()
{
    if (!_src)
        return;
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
