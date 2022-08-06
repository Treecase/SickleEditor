/**
 * Buffer.cpp - OpenGL buffer object.
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


/** Deleter for Buffer objects. (For use with shared_ptr and co.) */
void _buffer_delete(GLuint *buffer)
{
    glDeleteBuffers(1, buffer);
    delete buffer;
}


GLUtil::Buffer::Buffer(GLenum target, std::string label)
:   _id{new GLuint{0}, _buffer_delete}
,   target{target}
{
    glCreateBuffers(1, _id.get());
    if (!label.empty())
    {
        glObjectLabel(GL_BUFFER, *_id, (GLsizei)label.size(), label.c_str());
    }
}

GLuint GLUtil::Buffer::id() const
{
    return *_id;
}

void GLUtil::Buffer::bind() const
{
    glBindBuffer(target, *_id);
}
void GLUtil::Buffer::bind(GLenum target_)
{
    GLenum const bound = target_ == GL_NONE? target : target_;
    glBindBuffer(bound, *_id);
    target = bound;
}

void GLUtil::Buffer::unbind() const
{
    glBindBuffer(target, 0);
}
