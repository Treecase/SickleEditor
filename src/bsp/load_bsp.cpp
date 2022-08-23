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

size_t LumpMaxSize[] = {
    1024,       // Entities
    32767,      // Planes
    0x200000,   // Textures
    65535,      // Vertexes
    0x200000,   // Visibility
    32767,      // Nodes
    8192,       // TexInfo
    65535,      // Faces
    0x200000,   // Lighting
    32767,      // ClipNodes
    8192,       // Leafs
    65535,      // MarkSurfaces
    256000,     // Edges
    512000,     // SurfEdges
    400         // Models
};

struct M_Lump
{
    uint32_t fileofs, filelen;
};

struct M_Header
{
    uint32_t version;
    M_Lump lumps[LumpIndices::LumpCount];
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

/** Extract Textures from the Textures lump. */
std::vector<BSP::Texture> extract_textures(std::vector<uint8_t> const &lump)
{
    std::vector<BSP::Texture> out{};

    size_t lumpidx = 0;
    auto lptr = lump.data();

    // mipheader_t
    // .numtex
    int32_t numtex;
    memcpy(&numtex, lptr + lumpidx, 4);
    lumpidx += 4;
    // .offset
    std::vector<int32_t> offset{};
    offset.reserve(numtex);
    for (int i = 0; i < numtex; ++i)
    {
        int32_t o;
        memcpy(&o, lptr + lumpidx, 4);
        lumpidx += 4;
        offset.push_back(o);
    }

    for (auto const &o : offset)
    {
        lumpidx = o;

        // miptex_t
        // .name
        BSP::Texture t{};
        char name[16];
        memcpy(name, lptr + lumpidx, 16);
        t.name = name;
        lumpidx += 16;
        // .width
        uint32_t width;
        memcpy(&width, lptr + lumpidx, 4);
        t.width = width;
        lumpidx += 4;
        // .height
        uint32_t height;
        memcpy(&height, lptr + lumpidx, 4);
        t.height = height;
        lumpidx += 4;
        // .offset1
        uint32_t offset1;
        memcpy(&offset1, lptr + lumpidx, 4);
        lumpidx += 4;
        // .offset2
        uint32_t offset2;
        memcpy(&offset2, lptr + lumpidx, 4);
        lumpidx += 4;
        // .offset4
        uint32_t offset4;
        memcpy(&offset4, lptr + lumpidx, 4);
        lumpidx += 4;
        // .offset8
        uint32_t offset8;
        memcpy(&offset8, lptr + lumpidx, 4);

        // load full size texture
        lumpidx = o + offset1;
        auto tex = new uint8_t[t.width * t.height];
        memcpy(tex, lptr + lumpidx, t.width * t.height);
        t.tex1.reset(tex);

        // load half size texture
        lumpidx = o + offset2;
        tex = new uint8_t[t.width/2 * t.height/2];
        memcpy(tex, lptr + lumpidx, t.width/2 * t.height/2);
        t.tex2.reset(tex);

        // load quarter size texture
        lumpidx = o + offset4;
        tex = new uint8_t[t.width/4 * t.height/4];
        memcpy(tex, lptr + lumpidx, t.width/4 * t.height/4);
        t.tex4.reset(tex);

        // load eighth size texture
        lumpidx = o + offset8;
        tex = new uint8_t[t.width/8 * t.height/8];
        memcpy(tex, lptr + lumpidx, t.width/8 * t.height/8);
        t.tex8.reset(tex);

        out.push_back(t);
    }
    return out;
}


/** Load a .bsp file. */
BSP::BSP BSP::load_bsp(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw std::runtime_error{"Failed to open '" + path + "'"};

    // Read header
    M_Header hdr;
    f.read((char*)&hdr, 124);
    if (hdr.version != 30)
        throw std::runtime_error{
            path + " is version " + std::to_string(hdr.version)
            + ", only 30 is supported"};

    // Read lumps
    auto entity_data = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Entities]);
    auto planes = readLump<Plane>(f, hdr.lumps[LumpIndices::Planes]);
    auto texture_data = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Textures]);
    auto vertexes = readLump<Vertex>(f, hdr.lumps[LumpIndices::Vertexes]);
    auto visibility = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Visibility]);
    auto nodes = readLump<Node>(f, hdr.lumps[LumpIndices::Nodes]);
    auto texinfo = readLump<TexInfo>(f, hdr.lumps[LumpIndices::Texinfo]);
    auto faces = readLump<Face>(f, hdr.lumps[LumpIndices::Faces]);
    auto lighting = readLump<uint8_t>(f, hdr.lumps[LumpIndices::Lighting]);
    auto clipnodes = readLump<ClipNode>(f, hdr.lumps[LumpIndices::Clipnodes]);
    auto leafs = readLump<Leaf>(f, hdr.lumps[LumpIndices::Leafs]);
    auto marksurfaces = readLump<uint16_t>(f, hdr.lumps[LumpIndices::Marksurfaces]);
    auto edges = readLump<Edge>(f, hdr.lumps[LumpIndices::Edges]);
    auto surfedges = readLump<int32_t>(f, hdr.lumps[LumpIndices::Surfedges]);
    auto models = readLump<Model>(f, hdr.lumps[LumpIndices::Models]);

    auto entities = parse_entities(std::string{(char*)entity_data.data()});
    auto textures = extract_textures(texture_data);

    f.close();

    // Original Half-Life has limits on the number of elements a map can have.
    assert(entities.size() < LumpMaxSize[LumpIndices::Entities]);
    assert(planes.size() < LumpMaxSize[LumpIndices::Planes]);
    assert(textures.size() < LumpMaxSize[LumpIndices::Textures]);
    assert(vertexes.size() < LumpMaxSize[LumpIndices::Vertexes]);
    // assert(visibility.size() < LumpMaxSize[LumpIndices::Visibility]);
    assert(nodes.size() < LumpMaxSize[LumpIndices::Nodes]);
    assert(texinfo.size() < LumpMaxSize[LumpIndices::Texinfo]);
    assert(faces.size() < LumpMaxSize[LumpIndices::Faces]);
    // assert(lighting.size() < LumpMaxSize[LumpIndices::Lighting]);
    assert(clipnodes.size() < LumpMaxSize[LumpIndices::Clipnodes]);
    assert(leafs.size() < LumpMaxSize[LumpIndices::Leafs]);
    assert(marksurfaces.size() < LumpMaxSize[LumpIndices::Marksurfaces]);
    assert(edges.size() < LumpMaxSize[LumpIndices::Edges]);
    assert(surfedges.size() < LumpMaxSize[LumpIndices::Surfedges]);
    assert(models.size() < LumpMaxSize[LumpIndices::Models]);

    return BSP{
        entities,
        planes,
        textures,
        vertexes,
        visibility,
        nodes,
        texinfo,
        faces,
        lighting,
        clipnodes,
        leafs,
        marksurfaces,
        edges,
        surfedges,
        models
    };
}
