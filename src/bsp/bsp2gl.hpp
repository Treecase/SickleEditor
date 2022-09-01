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
#include "../wad/load_wad.hpp"

#include <array>


namespace BSP
{
    /** Format for buffered vertex data. */
    struct VertexDef
    {
        GLfloat x, y, z;    // Position
        GLfloat s, t;       // UV
    };

    struct Mesh
    {
        size_t tex_idx;
        GLsizei count;
        void *indices;
    };

    struct GLBSP
    {
        std::vector<Mesh> meshes;
        std::shared_ptr<GLUtil::VertexArray> vao;
        std::shared_ptr<GLUtil::Buffer> vbo, ebo;
    };


    /** Convert from .bsp to an OpenGL format. */
    GLBSP bsp2gl(BSP const &bsp);

    /** Get textures from .bsp data. */
    std::vector<GLUtil::Texture> getTextures(BSP const &bsp, std::string const &game_dir);
}

#endif
