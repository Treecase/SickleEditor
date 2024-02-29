/**
 * Brush.cpp - World3D::Brush class.
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

#include <utils/gtkglutils.hpp>

#include <stdexcept>


World3D::Brush::PreDrawFunc World3D::Brush::predraw = [](auto, auto){};



GLUtil::Program &World3D::Brush::shader()
{
    static GLUtil::Program the_shader{
        std::vector{
            GLUtil::shader_from_resource("shaders/map.vert", GL_VERTEX_SHADER),
            GLUtil::shader_from_resource("shaders/map.frag", GL_FRAGMENT_SHADER),
        },
        "BrushShader"
    };
    return the_shader;
}



void World3D::Brush::render() const
{
    if (!_src || !_vao)
        return;

    shader().use();
    shader().setUniformS("model", glm::identity<glm::mat4>());
    shader().setUniformS("view", glm::identity<glm::mat4>());
    shader().setUniformS("projection", glm::identity<glm::mat4>());
    shader().setUniformS("tex", 0);
    shader().setUniformS("modulate", glm::vec3{1.0f, 1.0f, 1.0f});

    glActiveTexture(GL_TEXTURE0);

    _vao->bind();
    predraw(shader(), _src);
    for (auto const &face : _faces)
    {
        face->render();
        glDrawArrays(GL_TRIANGLE_FAN, face->offset(), face->count());
    }
}


void World3D::Brush::execute()
{
    push_queue([this](){render();});
}



void World3D::Brush::on_attach(Sickle::Componentable &obj)
{
    if (_src)
        throw std::logic_error{"already attached"};
    if (typeid(obj) != typeid(Sickle::Editor::Brush &))
        throw std::invalid_argument{"expected a Sickle::Editor::Brush"};

    _src = dynamic_cast<Sickle::Editor::Brush const *>(&obj);

    auto offset = 0;
    for (auto const &faceptr : _src->faces())
    {
        auto face = std::make_shared<Face>(faceptr, offset);
        _faces.push_back(face);

        offset += face->vertices().size();

        _signals.push_back(
            face->signal_vertices_changed().connect(
                sigc::bind(
                    sigc::mem_fun(*this, &Brush::_on_face_changed),
                    face)));
    }

    push_queue([this](){_init();});
}


void World3D::Brush::on_detach(Sickle::Componentable &obj)
{
    _src = nullptr;
    _faces.clear();
    for (auto conn : _signals)
        conn.disconnect();
    _signals.clear();
    _vao = nullptr;
    _vbo = nullptr;
    clear_queue();
}



void World3D::Brush::_init()
{
    std::vector<GLfloat> vbo_data{};
    for (auto const &face : _faces)
    {
        for (auto const &vertex : face->vertices())
        {
            auto v = vertex.as_vbo();
            vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
        }
    }

    _vao = std::make_shared<GLUtil::VertexArray>();
    _vbo = std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER);

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


void World3D::Brush::_sync_face(std::shared_ptr<Face> const &face)
{
    std::vector<GLfloat> vbo_data{};
    for (auto const &vertex : face->vertices())
    {
        auto v = vertex.as_vbo();
        vbo_data.insert(vbo_data.end(), v.cbegin(), v.cend());
    }
    _vbo->bind();
    _vbo->update(
        vbo_data,
        face->offset() * Vertex::ELEMENTS,
        face->count() * Vertex::ELEMENTS);
    _vbo->unbind();
}


void World3D::Brush::_on_face_changed(std::shared_ptr<Face> const &face)
{
    push_queue([this, face](){_sync_face(face);});
}
