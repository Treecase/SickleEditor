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

#include "Brush.hpp"
#include "Face.hpp"

#include <glm/gtx/rotate_vector.hpp>


World3D::Face::SlotPreDraw World3D::Face::predraw = [](auto, auto){};
sigc::signal<void(std::string)> World3D::Face::_signal_missing_texture{};


GLUtil::Program &World3D::Face::shader()
{
    return Brush::shader();
}



World3D::Face::Face(Sickle::Editor::FaceRef const &face, GLint offset)
:   _src{face}
,   _offset{offset}
,   _starting_rotation{_src->get_rotation()}
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
    if (!_src || !_texture)
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

    _texture->texture->bind();
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

    // The initial rotation value is aligned with the UV axes. Therefore we
    // must keep track of the initial value and subtract it from the current
    // value to get the "actual" rotation.
    auto const rotation_delta = _src->get_rotation() - _starting_rotation;

    auto const u_axis = glm::normalize(_src->get_u());
    auto const v_axis = glm::normalize(_src->get_v());

    auto const texture_size = (_texture
        ? glm::vec2{_texture->width, _texture->height}
        : glm::vec2{0, 0});

    _vertices.clear();
    for (auto const &vertex : _src->get_vertices())
    {
        glm::vec2 uv{glm::dot(vertex, u_axis), glm::dot(vertex, v_axis)};

        uv /= _src->get_scale();
        uv = glm::rotate(uv, -glm::radians(rotation_delta));
        uv += _src->get_shift();

        // Normalize coordinates for OpenGL.
        uv /= texture_size;

        _vertices.emplace_back(vertex, uv);
    }
    signal_vertices_changed().emit();
}


void World3D::Face::_sync_texture()
{
    if (!_src)
        return;
    _texture = Texture::create_for_name(_src->get_texture());
    _sync_vertices();
}
