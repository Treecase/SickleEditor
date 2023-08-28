/**
 * brush.cpp - World3D::Brush class.
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


World3D::Brush::Brush(
    Entity &parent,
    Sickle::Editor::Entity::BrushRef const &src)
:   _parent{parent}
,   _src{src}
,   _vao{std::make_shared<GLUtil::VertexArray>()}
,   _vbo{std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER)}
{
    std::vector<GLfloat> vbo_data{};
    for (auto faceptr : _src.lock()->faces)
    {
        auto &face = _faces.emplace_back(*this, faceptr);
        face.offset = vbo_data.size() / Vertex::ELEMENTS;
        for (auto const &vertex : face.vertices)
        {
            auto v = vertex.as_vbo();
            vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
        }
    }

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

    for (size_t i = 0; i < _faces.size(); ++i)
        _face_changed_connection.push_back(
            _faces[i].signal_verts_changed().connect(
                [this, i](){_on_face_changed(i);}));
}


World3D::Brush::Brush(Brush const &other)
:   _parent{other._parent}
,   _src{other._src}
,   _faces{other._faces}
,   _vao{other._vao}
,   _vbo{other._vbo}
{
    for (size_t i = 0; i < _faces.size(); ++i)
        _face_changed_connection.push_back(
            _faces[i].signal_verts_changed().connect(
                [this, i](){_on_face_changed(i);}));
}


World3D::Brush::~Brush()
{
    for (auto &conn : _face_changed_connection)
        conn.disconnect();
}


World3D::TextureManager &World3D::Brush::texman() const
{
    return _parent.texman();
}


bool World3D::Brush::is_selected() const
{
    return _src.lock()->is_selected();
}


void World3D::Brush::render() const
{
    _vao->bind();
    for (auto const &face : _faces)
    {
        face.texture.texture->bind();
        glDrawArrays(GL_TRIANGLE_FAN, face.offset, face.count());
    }
}


void World3D::Brush::_on_face_changed(size_t face_index)
{
    auto &face = _faces[face_index];
    std::vector<GLfloat> vbo_data{};
    for (auto const &vertex : face.vertices)
    {
        auto v = vertex.as_vbo();
        vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
    }
    _vbo->bind();
    _vbo->update(
        vbo_data,
        face.offset * Vertex::ELEMENTS,
        face.count() * Vertex::ELEMENTS);
    _vbo->unbind();
}
