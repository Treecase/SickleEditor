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


/* ===[ MapArea::GLBrush ]=== */
GLBrush::GLBrush(
    std::vector<GLPlane> const &planes, std::vector<GLfloat> const &vbodata,
    std::vector<GLuint> const &ebodata)
:   planes{planes}
,   vao{"BrushVAO"}
,   vbo{GL_ARRAY_BUFFER, "BrushVBO"}
,   ebo{GL_ELEMENT_ARRAY_BUFFER, "BrushEBO"}
{
    vao.bind();
    vbo.bind();
    vbo.buffer(GL_STATIC_DRAW, vbodata);
    ebo.bind();
    ebo.buffer(GL_STATIC_DRAW, ebodata);
    vao.enableVertexAttribArray(0, 3, GL_FLOAT, 5*sizeof(GLfloat), 0);
    vao.enableVertexAttribArray(
        1, 2, GL_FLOAT, 5*sizeof(GLfloat), 3*sizeof(GLfloat));
    ebo.unbind();
    vbo.unbind();
    vao.unbind();
}

/** Transform map Brush to GL brush. */
GLBrush *GLBrush::create_from_map_brush(MAP::Brush const &brush, MAP::TextureManager &textures)
{
    // Merge Plane mesh V/EBOs into Brush V/EBO.
    std::vector<GLPlane> planes{};
    std::vector<GLfloat> vbodata{};
    std::vector<GLuint> ebodata{};
    for (auto const &mesh : mesh_from_planes(brush, textures))
    {
        planes.push_back({
            *textures.at(mesh.tex).texture,
            (GLsizei)mesh.ebo.size(),
            (void *)(ebodata.size() * sizeof(GLuint))});
        size_t const index = vbodata.size() / 5;
        for (auto const &idx : mesh.ebo)
            ebodata.push_back(index + idx);
        vbodata.insert(vbodata.end(), mesh.vbo.cbegin(), mesh.vbo.cend());
    }
    return new GLBrush{planes, vbodata, ebodata};
}


/* ===[ MapArea ]=== */
Sickle::MapArea::MapArea(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder)
:   Gtk::GLArea{cobject}
,   m_refBuilder{refBuilder}
,   _map{}
,   _brushes{}
,   _shader{nullptr}
,   _camera{}
,   _wireframe{false}
,   _shift_multiplier{2.0f}
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

void Sickle::MapArea::set_map(MAP::Map const &map)
{
    MAP::Entity worldspawn;
    for (auto const &e : map.entities)
    {
        if (e.properties.at("classname") == "worldspawn")
        {
            worldspawn = e;
            break;
        }
    }

    auto const &wad = WAD::load(worldspawn.properties.at("wad"));
    MAP::TextureManager textures{wad};

    make_current();
    _brushes.clear();
    for (auto const &b : worldspawn.brushes)
        _brushes.emplace_back(GLBrush::create_from_map_brush(b, textures));
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

    static float const vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    static auto const vshader_src =\
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "void main()"
        "{"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
    static auto const fshader_src =\
        "#version 330 core\n"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
        "}";

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vshader_src, nullptr);
    glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fshader_src, nullptr);
    glCompileShader(fShader);
    _program = glCreateProgram();
    glAttachShader(_program, vShader);
    glAttachShader(_program, fShader);
    glLinkProgram(_program);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void Sickle::MapArea::on_unrealize()
{
    glDeleteProgram(_program);
    glDeleteBuffers(1, &_vbo);
}

bool Sickle::MapArea::on_render(Glib::RefPtr<Gdk::GLContext> const &context)
{
    throw_if_error();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_brushes.empty())
        return false;

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

    for (auto const &brush : _brushes)
    {
        brush->vao.bind();
        brush->ebo.bind();
        for (auto const &plane : brush->planes)
        {
            plane.texture.bind();
            glDrawElements(
                GL_TRIANGLE_FAN, plane.count, GL_UNSIGNED_INT, plane.indices);
        }
    }
    Gtk::GLArea::on_render(context);
    return true;
}
