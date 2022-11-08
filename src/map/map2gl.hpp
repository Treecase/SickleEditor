/**
 * map2gl.hpp - Convert .map data into OpenGL objects.
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

#ifndef _MAP2GL_HPP
#define _MAP2GL_HPP

#include "../wad/load_wad.hpp"
#include "../wad/lumps.hpp"
#include "../wad/TextureManager.hpp"
#include "load_map.hpp"

#include <glutils/glutils.hpp>

#include <memory>
#include <string>
#include <vector>


namespace MAP
{
    // TODO: move this into glutil?
    /**
     * A Mesh, containing a texture name, vertex data, and element buffer data.
     */
    struct Mesh
    {
        std::string tex;
        std::vector<GLfloat> vbo{};
        std::vector<GLuint> ebo{};
    };

    /** Wraps GLUtil's Texture, to keep some additional information we need. */
    struct MapTexture
    {
        std::shared_ptr<GLUtil::Texture> texture;
        int w, h;

        MapTexture();
        MapTexture(WAD::TexLump const &texlump);
    };


    typedef WAD::TextureManager<MapTexture> TextureManager;
    /** Create a Mesh from a Brush's Planes. */
    std::vector<Mesh> mesh_from_planes(
        MAP::Brush const &brush, TextureManager &textures);
};

#endif
