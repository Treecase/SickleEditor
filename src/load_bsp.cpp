/**
 * load_bsp.cpp - Load .bsp files.
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

#include "load_bsp.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <cassert> // temp
#include <cstdint>
#include <cstring>


enum LumpIndices
{
    Entities = 0,
    Planes = 1,
    Textures = 2,
    Vertexes = 3,
    Visibility = 4,
    Nodes = 5,
    Texinfo = 6,
    Faces = 7,
    Lighting = 8,
    Clipnodes = 9,
    Leafs = 10,
    Marksurfaces = 11,
    Edges = 12,
    Surfedges = 13,
    Models = 14,
    LumpCount = 15
};

struct M_Lump
{
    int32_t fileofs,
            filelen;
};

struct M_Header
{
    int32_t version;
    M_Lump lumps[LumpIndices::LumpCount];
};

#define MAX_MAP_HULLS 4
struct M_Model
{
    float mins[3], maxs[3];
    float origin[3];
    int32_t headnode[MAX_MAP_HULLS];
    int32_t visleafs;
    int32_t firstface, numfaces;
};

struct M_Vertex
{
    float point[3];
};

struct M_Plane
{
    float normal[3];
    float dist;
    int32_t type;
};

#define NUM_AMBIENTS 4
struct M_Leaf
{
    int32_t contents;
    int32_t visofs;
    int16_t mins[3];
    int16_t maxs[3];
    uint16_t firstmarksurface;
    uint16_t nummarksurface;
    uint8_t ambient_level[NUM_AMBIENTS];
};

struct M_Node
{
    int32_t planenum;
    int16_t children[2];
    int16_t mins[2];
    int16_t maxs[2];
    uint16_t firstface;
    uint16_t numfaces;
};

struct M_TexInfo
{
    float vecs[2][4];
    int32_t miptex;
    int32_t flags;
};

struct M_ClipNode
{
    int32_t planenum;
    int16_t children[2];
};

#define MAXLIGHTMAPS 4
struct M_Face
{
    int16_t planenum;
    int16_t side;
    int32_t firstedge;
    int16_t numedges;
    int16_t texinfo;
    uint8_t styles[MAXLIGHTMAPS];
    int32_t lightofs;
};

struct M_Edge
{
    uint16_t v[2];
};


/** Read a lump from .bsp data. */
template<typename T>
std::vector<T> readLump(std::istream &f, M_Lump const &lumpdef)
{
    assert(lumpdef.filelen % sizeof(T) == 0);
    size_t count = lumpdef.filelen / sizeof(T);
    std::vector<T> lumpdata{};
    auto buf = new T[count];
    f.seekg(lumpdef.fileofs);
    f.read((char*)buf, lumpdef.filelen);
    lumpdata.reserve(count);
    lumpdata.insert(lumpdata.end(), buf, buf + count);
    delete[] buf;
    return lumpdata;
}


/** Load a .bsp file. */
BSP::BSP BSP::load_bsp(std::string const &path)
{
    std::cout << "LoadBSP: " << path << "\n";

    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw std::runtime_error{"Failed to open '" + path + "'"};

    // Read header
    M_Header hdr;
    f.read((char*)&hdr, 124);
    std::cout << "Header version: " << hdr.version << "\n";
    if (hdr.version != 30)
        throw std::runtime_error{
            path + " is version " + std::to_string(hdr.version)
            + ", only 30 is supported"};

    // Read lumps
    auto entities = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Entities]);
    auto planes = readLump<M_Plane>(f, hdr.lumps[LumpIndices::Planes]);
    auto textures = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Textures]);
    auto vertexes = readLump<M_Vertex>(f, hdr.lumps[LumpIndices::Vertexes]);
    auto visibility = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Visibility]);
    auto nodes = readLump<M_Node>(f, hdr.lumps[LumpIndices::Nodes]);
    auto texinfo = readLump<M_TexInfo>(f, hdr.lumps[LumpIndices::Texinfo]);
    auto faces = readLump<M_Face>(f, hdr.lumps[LumpIndices::Faces]);
    auto lighting = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Lighting]);
    auto clipnodes = readLump<M_ClipNode>(f, hdr.lumps[LumpIndices::Clipnodes]);
    auto leafs = readLump<M_Leaf>(f, hdr.lumps[LumpIndices::Leafs]);
    auto marksurfaces = readLump<uint16_t>(f, hdr.lumps[LumpIndices::Marksurfaces]);
    auto edges = readLump<M_Edge>(f, hdr.lumps[LumpIndices::Edges]);
    auto surfedges = readLump<uint32_t>(f, hdr.lumps[LumpIndices::Surfedges]);
    auto models = readLump<M_Model>(f, hdr.lumps[LumpIndices::Models]);

    std::cout << "Entities: " << entities.size() << "\n";
    std::cout << "Planes: " << planes.size() << "\n";
    std::cout << "Textures: " << textures.size() << "\n";
    std::cout << "Vertexes: " << vertexes.size() << "\n";
    std::cout << "Visibility: " << visibility.size() << "\n";
    std::cout << "Nodes: " << nodes.size() << "\n";
    std::cout << "Texinfo: " << texinfo.size() << "\n";
    std::cout << "Faces: " << faces.size() << "\n";
    std::cout << "Lighting: " << lighting.size() << "\n";
    std::cout << "Clipnodes: " << clipnodes.size() << "\n";
    std::cout << "Leafs: " << leafs.size() << "\n";
    std::cout << "Marksurfaces: " << marksurfaces.size() << "\n";
    std::cout << "Edges: " << edges.size() << "\n";
    std::cout << "Surfedges: " << surfedges.size() << "\n";
    std::cout << "Models: " << models.size() << "\n";

    f.close();

    return BSP{0};
}
