/**
 * VertexArray.cpp - OpenGL vertex array object.
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

#include "glUtil.hpp"

#include <GL/glew.h>


/** Deleter for VertexArray objects. (For use with shared_ptr and co.) */
void _vertex_array_delete(GLuint *vertexarray)
{
    glDeleteVertexArrays(1, vertexarray);
    delete vertexarray;
}


GLUtil::VertexArray::VertexArray(std::string label)
:   _id{new GLuint{0}, _vertex_array_delete}
{
    glCreateVertexArrays(1, _id.get());
    if (!label.empty())
    {
        glObjectLabel(
            GL_VERTEX_ARRAY, *_id, (GLsizei)label.size(), label.c_str());
    }
}

GLuint GLUtil::VertexArray::id() const
{
    return *_id;
}

void GLUtil::VertexArray::bind() const
{
    glBindVertexArray(*_id);
}

void GLUtil::VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void GLUtil::VertexArray::enableVertexAttribArray(
    GLuint index, GLint components, GLenum dataType, GLsizei stride,
    size_t offset, bool normalized) const
{
    glVertexAttribPointer(
        index, components, dataType, normalized? GL_TRUE : GL_FALSE, stride,
        (void *)offset);
    glEnableVertexAttribArray(index);
}
