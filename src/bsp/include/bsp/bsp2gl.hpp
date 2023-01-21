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

#ifndef SE_BSP2GL_HPP
#define SE_BSP2GL_HPP

#include "load_bsp.hpp"
#include "wad/wad.hpp"

#include <glutils/glutils.hpp>

#include <array>


namespace BSP
{
    /** The lowest-level BSP GL object. */
    struct Mesh
    {
        GLUtil::Texture tex;
        GLsizei count;
        void *indices;
    };

    /** A model is made up of several Meshes. */
    struct GLModel
    {
        glm::vec3 position;
        std::vector<Mesh> meshes;
    };

    /** The BSP's GL representation. */
    class GLBSP
    {
    public:
        GLBSP();
        /** Convert from .bsp to an OpenGL format. */
        GLBSP(BSP const &bsp, std::string const &game_dir);

        /** Draw the GLBSP. */
        void render();

    private:
        std::vector<GLModel> models;
        std::shared_ptr<GLUtil::VertexArray> vao;
        std::shared_ptr<GLUtil::Buffer> vbo, ebo;
    };


    /** Get textures from .bsp data. */
    std::vector<GLUtil::Texture> getTextures(BSP const &bsp, std::string const &game_dir);
}

#endif
