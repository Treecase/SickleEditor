/**
 * glmap.hpp - Convert .map data into OpenGL objects.
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

#ifndef SE_GLMAP_HPP
#define SE_GLMAP_HPP

#include "../../../wad/load_wad.hpp"
#include "../../../wad/lumps.hpp"
#include "../../../wad/TextureManager.hpp"
#include "map/map.hpp"

#include <glutils/glutils.hpp>

#include <memory>
#include <string>
#include <vector>


namespace MAP
{
    /** Wraps GLUtil's Texture, to keep some additional information we need. */
    struct MapTexture
    {
        std::shared_ptr<GLUtil::Texture> texture;
        int w, h;

        MapTexture();
        MapTexture(WAD::TexLump const &texlump);
    };

    using TextureManager = WAD::TextureManager<MapTexture>;

    /** GL representation of a Brush Plane. */
    struct GLPlane
    {
        GLUtil::Texture texture;
        GLsizei count;
        void *indices;
    };

    /** GL representation of a Brush. */
    struct GLBrush
    {
        std::vector<GLPlane> planes;
        GLUtil::VertexArray vao;
        GLUtil::Buffer vbo, ebo;

        GLBrush(
            std::vector<GLPlane> const &planes,
            std::vector<GLfloat> const &vbodata,
            std::vector<GLuint> const &ebodata);

        /** Create a new GLBrush from a Brush. */
        static GLBrush *new_from_brush(Brush const &brush, TextureManager &textures);
    };

    class GLMap
    {
    public:
        GLMap();
        /** Transform map Brush to GL brush. */
        GLMap(Map const &map);

        /** Draw a GLMap. */
        void render();

    private:
        std::vector<std::shared_ptr<GLBrush>> _brushes;
    };
};

#endif
