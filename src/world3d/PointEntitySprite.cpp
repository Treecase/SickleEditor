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

using namespace World3D;


PointEntitySprite::PointEntitySprite()
{
    push_queue([this](){_init_construct();});
}


void PointEntitySprite::render() const
{
    if (!_vao || !_sprite)
        return;

    glActiveTexture(GL_TEXTURE0);
    _vao->bind();
    _sprite->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    _vao->unbind();
}


void PointEntitySprite::execute()
{
    push_queue([this](){render();});
}



void PointEntitySprite::on_attach(Sickle::Componentable &obj)
{
    push_queue([this](){_init();});
}


void PointEntitySprite::on_detach(Sickle::Componentable &obj)
{
    _sprite.reset();
}



void PointEntitySprite::_init_construct()
{
    std::vector<GLfloat> const vbo_data{
        // position       |  UVs
        -16.0f, 256.0f, +16.0f,  0.0f, 0.0f, // left top
        -16.0f, 256.0f, -16.0f,  0.0f, 1.0f, // left bottom
        +16.0f, 256.0f, +16.0f,  1.0f, 0.0f, // right top
        // +16.0f, 256.0f, -16.0f,  1.0f, 1.0f, // right bottom
    };

    _vao = std::make_shared<GLUtil::VertexArray>();
    _vbo = std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER);

    _vao->bind();
    _vbo->bind();
    _vbo->buffer(GL_STATIC_DRAW, vbo_data);
    _vao->enableVertexAttribArray(0, 3, GL_FLOAT, 5 * sizeof(GLfloat));
    _vao->enableVertexAttribArray(
        1,
        2,
        GL_FLOAT,
        5 * sizeof(GLfloat),
        3 * sizeof(GLfloat));
    _vbo->unbind();
    _vao->unbind();
}


void PointEntitySprite::_init()
{
    GLubyte const blank_texture[] = {
        0xff,0x00,0x00,0xff, 0x00,0xff,0x00,0xff,
        0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
    };

    _sprite = std::make_shared<GLUtil::Texture>(GL_TEXTURE_2D);
    _sprite->bind();
    _sprite->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _sprite->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(
        _sprite->type(), 0, GL_RGBA,
        2, 2, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, blank_texture);
    _sprite->unbind();
}
