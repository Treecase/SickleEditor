/**
 * mdl2gl.hpp - Convert loaded MDL data into OpenGL objects.
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

#ifndef _MDL2GL_HPP
#define _MDL2GL_HPP

#include "glUtil.hpp"
#include "load_model.hpp"


/** Format for buffered vertex data. */
struct VertexDef
{
    GLfloat x, y, z;    // Position
    GLfloat s, t;       // UV
    GLfloat r, g, b;    // Vertex color
};

struct MeshDef
{
    // VBO data.
    std::vector<VertexDef> vertices;
    // EBO data.
    std::vector<GLuint> indices;
};

struct ModelDef
{
    // Model meshes.
    std::vector<MeshDef> meshes;
};

struct GLMDL
{
    std::vector<GLsizei> count;
    std::vector<void *> indices;
    std::vector<size_t> texture;
    std::shared_ptr<GLUtil::VertexArray> vao;
    std::shared_ptr<GLUtil::Buffer> vbo, ebo;
};


/** Convert from MDL texture format to an OpenGL texture object. */
GLUtil::Texture texture2GLTexture(MDL::Texture const &texture);

/** Convert from MDL Model format to an OpenGL VAO. */
GLMDL model2vao(MDL::Model const &model);

#endif
