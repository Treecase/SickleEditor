/**
 * Brush.cpp - World3D::Brush class.
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

#include "world3d/Brush.hpp"


World3D::Brush::Brush(Sickle::Editor::BrushRef const &src)
:   sigc::trackable{}
,   _src{src}
,   _vao{std::make_shared<GLUtil::VertexArray>()}
,   _vbo{std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER)}
{
    std::vector<GLfloat> vbo_data{};
    for (auto const &faceptr : _src->faces)
    {
        auto const offset = vbo_data.size() / Vertex::ELEMENTS;
        auto &face = _faces.emplace_back(
            std::make_shared<Face>(faceptr, offset));
        for (auto const &vertex : face->vertices)
        {
            auto v = vertex.as_vbo();
            vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
        }
        faceptr->signal_vertices_changed().connect(
            sigc::bind(
                sigc::mem_fun(*this, &Brush::_on_face_changed), face));
    }

    _src->property_real().signal_changed().connect(
        sigc::mem_fun(*this, &Brush::_on_real_changed));

    _vao->bind();

    _vbo->bind();
    _vbo->buffer(GL_STATIC_DRAW, vbo_data);

    // NOTE: These MUST match Vertex::as_vbo() format!
    // Attrib 0: Vertex positions
    _vao->enableVertexAttribArray(0, 3, GL_FLOAT,
        Vertex::ELEMENTS * sizeof(GLfloat), 0);
    // Attrib 1: UVs
    _vao->enableVertexAttribArray(
        1, 2, GL_FLOAT,
        Vertex::ELEMENTS * sizeof(GLfloat), 3 * sizeof(GLfloat));

    _vbo->unbind();
    _vao->unbind();
}


bool World3D::Brush::is_selected() const
{
    return _src->is_selected();
}


void World3D::Brush::render() const
{
    _vao->bind();
    for (auto const &face : _faces)
    {
        face->texture.texture->bind();
        glDrawArrays(GL_TRIANGLE_FAN, face->offset, face->count());
    }
}


void World3D::Brush::_on_face_changed(std::shared_ptr<Face> const &face)
{
    std::vector<GLfloat> vbo_data{};
    for (auto const &vertex : face->vertices)
    {
        auto v = vertex.as_vbo();
        vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
    }
    _vbo->bind();
    _vbo->update(
        vbo_data,
        face->offset * Vertex::ELEMENTS,
        face->count() * Vertex::ELEMENTS);
    _vbo->unbind();
}


void World3D::Brush::_on_real_changed()
{
    if (!_src->property_real().get_value())
        signal_deleted().emit();
}
