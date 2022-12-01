/**
 * MapArea.hpp - Sickle editor main window GLArea.
 * Copyright (C) 2022 Trevor Last
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

#include "MapArea.hpp"
#include "../../../fgd/fgd.hpp"

#include <iostream>


/* ===[ MapArea ]=== */
Sickle::MapArea::MapArea(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder)
:   Glib::ObjectBase{typeid(MapArea)}
,   Gtk::GLArea{cobject}
,   m_refBuilder{refBuilder}
,   _glmap{}
,   _shader{nullptr}
,   _camera{}
,   _prop_wireframe{false}
,   _prop_shift_multiplier{2.0f}
,   _transform{
        {0.0f, 0.0f, 0.0f},
        {glm::radians(-90.0f), 0.0f, 0.0f},
        {0.005f, 0.005f, 0.005f}}
{
    set_required_version(4, 3);
    set_use_es(false);
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(640, 480);
    set_auto_render(true);
}

void Sickle::MapArea::set_map(MAP::Map const *map)
{
    make_current();
    if (map)
        _glmap = MAP::GLMap{*map};
    else
        _glmap = MAP::GLMap{};
    queue_render();
}

void Sickle::MapArea::on_realize()
{
    Gtk::GLArea::on_realize();
    make_current();
    throw_if_error();

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        throw std::runtime_error{
            "glewInit - " + std::string{(char *)glewGetErrorString(error)}};
    }
    if (glewIsSupported("GL_VERSION_4_3") == GL_FALSE)
        throw std::runtime_error{"GLEW: OpenGL Version 4.3 not supported"};

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    _shader.reset(
        new GLUtil::Program{
            {
                GLUtil::shader_from_file("shaders/map.vert", GL_VERTEX_SHADER),
                GLUtil::shader_from_file("shaders/map.frag", GL_FRAGMENT_SHADER)
            },
            "MapShader"
        }
    );
}

void Sickle::MapArea::on_unrealize()
{
}

bool Sickle::MapArea::on_render(Glib::RefPtr<Gdk::GLContext> const &context)
{
    throw_if_error();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup projection matrix.
    auto const projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        get_width() / (float)get_height(),
        0.1f, 1000.0f);

    auto const modelMatrix = _transform.getMatrix();

    // Draw models.
    _shader->use();
    glActiveTexture(GL_TEXTURE0);
    _shader->setUniformS("view", _camera.getViewMatrix());
    _shader->setUniformS("projection", projectionMatrix);
    _shader->setUniformS("tex", 0);
    _shader->setUniformS("model", modelMatrix);
    _glmap.render();
    Gtk::GLArea::on_render(context);
    return true;
}
