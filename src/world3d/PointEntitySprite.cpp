/**
 * PointEntitySprite.hpp - OpenGL Editor::Entity view for PointClass entities.
 *                         Renders as a billboarded sprite.
 * Copyright (C) 2024 Trevor Last
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

#include "world3d/Entity.hpp"

#include <utils/gtkglutils.hpp>

using namespace World3D;


PointEntitySprite::PreDrawFunc PointEntitySprite::predraw = [](auto, auto){};


GLUtil::Program &PointEntitySprite::shader()
{
    static GLUtil::Program the_shader{
        std::vector{
            GLUtil::shader_from_resource(
                "shaders/billboard.vert",
                GL_VERTEX_SHADER),
            GLUtil::shader_from_resource(
                "shaders/map.frag",
                GL_FRAGMENT_SHADER),
        },
        "PointEntitySpriteShader"
    };
    return the_shader;
}



PointEntitySprite::PointEntitySprite()
{
    push_queue([this](){_init_construct();});
}


void PointEntitySprite::render() const
{
    if (!_vao || !_sprite)
        return;

    std::stringstream origin_str{_src->get_property("origin")};
    glm::vec3 origin{};
    origin_str >> origin.x >> origin.y >> origin.z;

    ShaderParams params{};
    params.model = glm::identity<glm::mat4>();
    params.view = glm::identity<glm::mat4>();
    params.projection = glm::identity<glm::mat4>();

    predraw(params, _src);

    shader().use();
    shader().setUniformS("scale", glm::vec2{0.1f, 0.1f});
    shader().setUniformS("position", origin);
    shader().setUniformS("model", params.model);
    shader().setUniformS("view", params.view);
    shader().setUniformS("projection", params.projection);
    shader().setUniformS("tex", 0);
    shader().setUniformS("modulate",
        _src->is_selected()
        ? glm::vec3{1.0f, 0.0f, 0.0f}
        : glm::vec3{1.0f, 1.0f, 1.0f});

    glActiveTexture(GL_TEXTURE0);
    _sprite->bind();

    _vao->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _vao->unbind();
}


void PointEntitySprite::execute()
{
    push_queue([this](){render();});
}



void PointEntitySprite::on_attach(Sickle::Componentable &obj)
{
    if (_src)
        throw std::logic_error{"already attached"};

    _src = &dynamic_cast<Sickle::Editor::Entity &>(obj);
    if (_src->classinfo().type != "PointClass")
        throw std::invalid_argument{"must be PointClass"};

    push_queue([this](){_init();});
}


void PointEntitySprite::on_detach(Sickle::Componentable &obj)
{
    _src = nullptr;
    _sprite.reset();
    clear_queue();
}



void PointEntitySprite::_init_construct()
{
    std::vector<GLfloat> const vbo_data{
        // position |  UVs
        -0.5f, +0.5f,  0.0f, 0.0f, // left top
        -0.5f, -0.5f,  0.0f, 1.0f, // left bottom
        +0.5f, +0.5f,  1.0f, 0.0f, // right top
        +0.5f, -0.5f,  1.0f, 1.0f, // right bottom
    };

    _vao = std::make_shared<GLUtil::VertexArray>();
    _vbo = std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER);

    _vao->bind();
    _vbo->bind();
    _vbo->buffer(GL_STATIC_DRAW, vbo_data);
    _vao->enableVertexAttribArray(0, 2, GL_FLOAT, 4 * sizeof(GLfloat));
    _vao->enableVertexAttribArray(
        1,
        2,
        GL_FLOAT,
        4 * sizeof(GLfloat),
        2 * sizeof(GLfloat));
    _vbo->unbind();
    _vao->unbind();
}


void PointEntitySprite::_init()
{
    // TODO: Load the sprite to be displayed.
    GLsizei const texture_width = 4;
    GLsizei const texture_height = 4;
    GLubyte const blank_texture[texture_width * texture_height * 4] = {
        0xff,0x00,0xff,0xff, 0x00,0x00,0x00,0xff,
        0xff,0x00,0xff,0xff, 0x00,0x00,0x00,0xff,
        0x00,0x00,0x00,0xff, 0xff,0x00,0xff,0xff,
        0x00,0x00,0x00,0xff, 0xff,0x00,0xff,0xff,
        0xff,0x00,0xff,0xff, 0x00,0x00,0x00,0xff,
        0xff,0x00,0xff,0xff, 0x00,0x00,0x00,0xff,
        0x00,0x00,0x00,0xff, 0xff,0x00,0xff,0xff,
        0x00,0x00,0x00,0xff, 0xff,0x00,0xff,0xff,
    };

    _sprite = std::make_shared<GLUtil::Texture>(GL_TEXTURE_2D);
    _sprite->bind();
    _sprite->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _sprite->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(
        _sprite->type(), 0, GL_RGBA,
        texture_width, texture_height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, blank_texture);
    _sprite->unbind();
}
