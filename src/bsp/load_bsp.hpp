/**
 * load_bsp.hpp - Load .bsp files.
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

#ifndef _LOAD_BSP
#define _LOAD_BSP

#include "entities_lump.hpp"

#include <memory>
#include <string>
#include <vector>


namespace BSP
{
    using vec3 = float[3];

    struct BBox
    {
        vec3 min, max;
    };

    struct BBoxShort
    {
        int16_t min[3], max[3];
    };


    /** Mipmapped texture. */
    struct Texture
    {
        std::string name;
        size_t width, height;
        std::shared_ptr<uint8_t> tex1; // full size mipmap
        std::shared_ptr<uint8_t> tex2; // half size mipmap
        std::shared_ptr<uint8_t> tex4; // quarter size mipmap
        std::shared_ptr<uint8_t> tex8; // eighth size mipmap
    };

    /** Texture info. */
    struct TexInfo
    {
        vec3 sVector;
        float sDist;
        vec3 tVector;
        float tDist;
        int32_t texture; // pointer into textures
        int32_t flags;
    };

    /** BSP split plane. */
    struct Plane
    {
        vec3 normal;
        float dist;
        int32_t type;
    };

    /** Map vertex. */
    struct Vertex
    {
        float x, y, z;
    };

    /** Map edge. */
    struct Edge
    {
        uint16_t start, end; // pointers into vertices
    };

    /** Map polygon. */
    struct Face
    {
        uint16_t plane; // pointer into planes
        uint16_t side;
        int32_t surfedge; // pointer into surfedges
        uint16_t surfedge_num;
        uint16_t texinfo; // pointer into texinfo
        uint8_t styles[4];
        int32_t lightmap;
    };

    /** BSP Node. */
    struct Node
    {
        uint32_t plane; // pointer into planes
        uint16_t front, back;
        BBoxShort box;
        uint16_t face; // pointer into faces
        uint16_t face_num;
    };

    /** ClipNode. */
    struct ClipNode
    {
        uint32_t plane; // pointer into planes
        int16_t front, back;
    };

    /** BSP Leaf Node. */
    struct Leaf
    {
        int32_t type;
        int32_t vislist; // pointer into visibility
        BBoxShort bbox;
        uint16_t marksurface; // pointer into marksurfaces
        uint16_t marksurface_num;
        uint8_t ambient_level[4];
    };

    /** Model. */
    struct Model
    {
        BBox bbox;
        vec3 origin;
        int32_t node_id[4];
        int32_t numleafs;
        int32_t face; // pointer into faces
        int32_t numfaces;
    };

    /** Loaded .bsp data. */
    struct BSP
    {
        std::vector<Entity> entities;
        std::vector<Plane> planes;
        std::vector<Texture> textures; // aka miptex
        std::vector<Vertex> vertices;  // aka vertexes
        std::vector<uint8_t> visibility; // aka visilist
        std::vector<Node> nodes;
        std::vector<TexInfo> texinfo;
        std::vector<Face> faces;
        std::vector<uint8_t> lightmaps; // aka lighting
        std::vector<ClipNode> clipnodes;
        std::vector<Leaf> leaves;
        std::vector<uint16_t> marksurfaces; // aka lface
        std::vector<Edge> edges;
        std::vector<int32_t> surfedges; // aka ledges
        std::vector<Model> models;
    };

    /** Load a .bsp file. */
    BSP load(std::string const &path);
}

#endif
