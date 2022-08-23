/**
 * bsp2gl.hpp - Convert .bsp data into OpenGL objects.
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

#ifndef _BSP2GL_HPP
#define _BSP2GL_HPP

#include "glUtil.hpp"
#include "load_bsp.hpp"


/** Format for buffered vertex data. */
struct BSPVertexDef
{
    GLfloat x, y, z;    // Position
    GLfloat s, t;       // UV
};

struct GLBSP
{
    std::vector<BSPVertexDef> vertices;
    std::vector<GLuint> indices;
    std::shared_ptr<GLUtil::VertexArray> vao;
    std::shared_ptr<GLUtil::Buffer> vbo, ebo;
};


/** Convert from .bsp to an OpenGL format. */
GLBSP bsp2gl(BSP::BSP const &bsp);

#endif
