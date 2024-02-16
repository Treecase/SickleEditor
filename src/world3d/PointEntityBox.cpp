/**
 * PointEntityBox.hpp - OpenGL Editor::Entity view for PointClass entities.
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

#include <glm/glm.hpp>

#include <stdexcept>
#include <vector>

using namespace World3D;


PointEntityBox::PreDrawFunc PointEntityBox::predraw = [](auto){};


void PointEntityBox::render() const
{
    if (!_src || !_vao)
        return;

    _vao->bind();
    predraw(_src);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(255);
    glDrawElements(GL_TRIANGLE_STRIP, 20, GL_UNSIGNED_BYTE, (void *)0);
    glDisable(GL_PRIMITIVE_RESTART);
}


void PointEntityBox::execute()
{
    push_queue([this](){render();});
}



void PointEntityBox::on_attach(Sickle::Componentable &obj)
{
    if (_src)
        throw std::logic_error{"already attached"};
    if (typeid(obj) != typeid(Sickle::Editor::Entity &))
        throw std::invalid_argument{"expected Sickle::Editor::Entity"};

    _src = dynamic_cast<Sickle::Editor::Entity const *>(&obj);
    if (_src->classinfo().type != "PointClass")
        throw std::invalid_argument{"must be PointClass"};

    push_queue([this](){_init();});
}


void PointEntityBox::on_detach(Sickle::Componentable &)
{
    _src = nullptr;
    _vao = nullptr;
    _vbo = nullptr;
    _ebo = nullptr;
    clear_queue();
}



void PointEntityBox::_init()
{
    auto const A = DEFAULT_BOX_SIZE * glm::vec3{-0.5,-0.5,-0.5};
    auto const B = DEFAULT_BOX_SIZE * glm::vec3{+0.5,+0.5,+0.5};

    std::vector<GLfloat> const vbo_data{
        A.x, A.y, A.z, // 0 left front bottom
        B.x, A.y, A.z, // 1 right front bottom
        A.x, B.y, A.z, // 2 left back bottom
        B.x, B.y, A.z, // 3 right back bottom
        A.x, A.y, B.z, // 4 left front top
        A.x, B.y, B.z, // 5 left back top
        B.x, A.y, B.z, // 6 right front top
        B.x, B.y, B.z  // 7 right back top
    };

    // Rendered as triangle strips using primitive restart index 255.
    std::vector<GLubyte> const ebo_data{
        4,0,6, 1, // front face
        7, 3,     // right face
        5, 2,     // back face
        4, 0,     // left face
        255,
        5,4,7,6,  // top face
        255,
        0,2,1, 3  // bottom face
    };

    // TODO: PointClasses without a size() class property should share GPU data.
    _vao = std::make_shared<GLUtil::VertexArray>();
    _vbo = std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER);
    _ebo = std::make_shared<GLUtil::Buffer>(GL_ELEMENT_ARRAY_BUFFER);

    _vao->bind();

    _vbo->bind();
    _vbo->buffer(GL_STATIC_DRAW, vbo_data);

    _ebo->bind();
    _ebo->buffer(GL_STATIC_DRAW, ebo_data);

    _vao->enableVertexAttribArray(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);

    _vbo->unbind();
    _vao->unbind();
}
