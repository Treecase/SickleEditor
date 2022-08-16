/**
 * load_model.hpp - Load .mdl files.
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

#ifndef _LOAD_MODEL
#define _LOAD_MODEL

#include <array>
#include <string>
#include <vector>

#include <cstdint>


namespace MDL
{
struct Vec3
{
    float x, y, z;
};


/** MDL Texture data. */
struct Texture
{
    std::string name;
    int w, h;
    std::vector<uint8_t> data;
    std::array<std::array<uint8_t, 3>, 256> palette;
};

/** MDL Vertex. */
struct Vertex
{
    int position_index;
    int light_index;
    int uv_s,
        uv_t;
};

/** MDL Tricmds. */
struct Tricmd
{
    bool mode; // true = triangle fan, false = triangle strip
    std::vector<Vertex> vertices;
};

/** MDL Mesh. */
struct Mesh
{
    std::vector<Tricmd> tricmds;
    uint16_t skinref;
};

/** MDL Model. */
struct MDLModel
{
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<Vec3> vertices;
};

/** MDL BodyPart. */
struct BodyPart
{
    std::string name;
    std::vector<MDLModel> models;
};

/** Loaded MDL data. */
struct Model
{
    std::string name;
    std::vector<BodyPart> bodyparts;
    std::vector<Texture> textures;
    std::vector<uint16_t> skinref;
};


/** Load a .mdl file. */
Model load_mdl(std::string const &path);
}

#endif
