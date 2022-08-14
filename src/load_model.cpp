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

#include "load_model.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>

#include <cstdint>
#include <cstring>


typedef float vec3[3];

struct M_Header
{
    char id[4];
    uint32_t version;

    char name[64];
    uint32_t length;

    vec3 eyePosition;
    vec3 min;
    vec3 max;

    vec3 bbmin;
    vec3 bbmax;

    uint32_t flags;

    uint32_t numbones;
    uint32_t boneindex;

    uint32_t numbonecontrollers;
    uint32_t bonecontrollerindex;

    uint32_t numhitboxes;
    uint32_t hitboxindex;

    uint32_t numseq;
    uint32_t seqindex;

    uint32_t numseqgroups;
    uint32_t seqgroupindex;

    uint32_t numtextures;
    uint32_t textureindex;
    uint32_t texturedataindex;

    uint32_t numskinref;
    uint32_t numskinfamilies;
    uint32_t skinindex;

    uint32_t numbodyparts;
    uint32_t bodypartindex;

    uint32_t numattachments;
    uint32_t attachmentindex;

    uint32_t soundtable;
    uint32_t soundindex;
    uint32_t soundgroups;
    uint32_t soundgroupindex;

    uint32_t numtransitions;
    uint32_t transitionindex;
};

struct M_BodyPart
{
    char name[64];
    uint32_t nummodels;
    uint32_t base;
    uint32_t modelindex;
};

struct M_Texture
{
    char name[64];
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    uint32_t index;
};

struct M_Model
{
    char name[64];
    uint32_t type;
    float boundingradius;
    uint32_t nummesh;
    uint32_t meshindex;
    uint32_t numverts;
    uint32_t vertinfoindex;
    uint32_t vertindex;
    uint32_t numnorms;
    uint32_t norminfoindex;
    uint32_t normindex;
    uint32_t numgroups;
    uint32_t groupindex;
};

struct M_Mesh
{
    uint32_t numtris;
    uint32_t triindex;
    uint32_t skinref;
    uint32_t numnorms;
    uint32_t normindex;
};

struct LoadContext
{
    std::ifstream f;
    M_Header hdr;
};


/** Load a texture. */
MDL::Texture _load_texture(LoadContext &con, uint32_t texture)
{
    MDL::Texture result{};

    M_Texture tex;
    con.f.seekg(con.hdr.textureindex + 80 * texture);
    con.f.read((char *)&tex.name, 64);
    con.f.read((char *)&tex.flags, 4);
    con.f.read((char *)&tex.width, 4);
    con.f.read((char *)&tex.height, 4);
    con.f.read((char *)&tex.index, 4);

    result.name = std::string{tex.name};
    result.w = tex.width;
    result.h = tex.height;
    auto const buf = new char[tex.width * tex.height];
    con.f.seekg(tex.index);
    con.f.read(buf, tex.width * tex.height);
    con.f.read((char *)result.palette.data(), 256 * 3);

    result.data.reserve(tex.width * tex.height);
    for (size_t i = 0; i < tex.width * tex.height; ++i)
        result.data.push_back(buf[i]);
    delete[] buf;

    return result;
}

/** Load a Mesh's tricmds from .mdl data. */
void _load_mesh_tricmds(LoadContext &con, M_Mesh &mesh, MDL::Mesh &msh)
{
    struct M_Vertex
    {
        uint16_t modelposition;
        uint16_t modellight;
        uint16_t uv_s,
                 uv_t;
    };
    con.f.seekg(mesh.triindex);

    int16_t numverts;
    con.f.read((char*)&numverts, 2);
    int16_t n = numverts < 0? -numverts : numverts;

    while (n != 0)
    {
        MDL::Tricmd tri;
        for (int16_t j = 0; j < n; ++j)
        {
            M_Vertex vertex;
            con.f.read((char*)&vertex.modelposition, 2);
            con.f.read((char*)&vertex.modellight, 2);
            con.f.read((char*)&vertex.uv_s, 2);
            con.f.read((char*)&vertex.uv_t, 2);
            tri.vertices.push_back({
                vertex.modelposition, vertex.modellight,
                vertex.uv_s, vertex.uv_t});
        }
        tri.mode = numverts < 0;
        msh.tricmds.push_back(tri);

        con.f.read((char*)&numverts, 2);
        n = numverts < 0? -numverts : numverts;
    }
}

/** Load a Model's Meshes from .mdl data. */
void _load_model_meshes(LoadContext &con, M_Model &model, MDL::MDLModel &mdl)
{
    for (uint32_t i = 0; i < model.nummesh; ++i)
    {
        M_Mesh mesh;
        con.f.seekg(model.meshindex + 20 * i);
        con.f.read((char*)&mesh.numtris, 4);
        con.f.read((char*)&mesh.triindex, 4);
        con.f.read((char*)&mesh.skinref, 4);
        con.f.read((char*)&mesh.numnorms, 4);
        con.f.read((char*)&mesh.normindex, 4);
        mdl.meshes.push_back({});
        _load_mesh_tricmds(con, mesh, mdl.meshes.at(i));
    }
}

/** Load a Model's vertices from .mdl data. */
void _load_model_vertices(LoadContext &con, M_Model &model, MDL::MDLModel &mdl)
{
    con.f.seekg(model.vertindex);
    for (uint32_t i = 0; i < model.numverts; ++i)
    {
        mdl.vertices.push_back({0.0f, 0.0f, 0.0f});
        auto &vertex = mdl.vertices.at(i);
        con.f.read((char*)&vertex.x, 4);
        con.f.read((char*)&vertex.y, 4);
        con.f.read((char*)&vertex.z, 4);
    }
}

/** Load a BodyPart's Models from .mdl data. */
void _load_bodypart_models(LoadContext &con, M_BodyPart &bodypart, MDL::BodyPart &bp)
{
    for (uint32_t i = 0; i < bodypart.nummodels; ++i)
    {
        M_Model model{};
        con.f.seekg(bodypart.modelindex + 112 * i);
        con.f.read(model.name, 64);
        con.f.read((char*)&model.type, 4);
        con.f.read((char*)&model.boundingradius, 4);
        con.f.read((char*)&model.nummesh, 4);
        con.f.read((char*)&model.meshindex, 4);
        con.f.read((char*)&model.numverts, 4);
        con.f.read((char*)&model.vertinfoindex, 4);
        con.f.read((char*)&model.vertindex, 4);
        con.f.read((char*)&model.numnorms, 4);
        con.f.read((char*)&model.norminfoindex, 4);
        con.f.read((char*)&model.normindex, 4);
        con.f.read((char*)&model.numgroups, 4);
        con.f.read((char*)&model.groupindex, 4);
        bp.models.push_back({model.name, {}, {}});
        _load_model_meshes(con, model, bp.models.at(i));
        _load_model_vertices(con, model, bp.models.at(i));
    }
}

/** Load BodyParts from .mdl data. */
void _load_bodyparts(LoadContext &con, MDL::Model &result)
{
    for (uint32_t i = 0; i < con.hdr.numbodyparts; ++i)
    {
        M_BodyPart bodypart{};
        con.f.seekg(con.hdr.bodypartindex + 76 * i);
        con.f.read(bodypart.name, 64);
        con.f.read((char*)&bodypart.nummodels, 4);
        con.f.read((char*)&bodypart.base, 4);
        con.f.read((char*)&bodypart.modelindex, 4);
        result.bodyparts.push_back({bodypart.name, {}});
        _load_bodypart_models(con, bodypart, result.bodyparts.at(i));
    }
}


MDL::Model MDL::load_mdl(std::string const &path)
{
    MDL::Model result{};

    LoadContext con{std::ifstream{path, std::ios::in | std::ios::binary}, {}};

    con.f.exceptions(std::ifstream::failbit); // temp, enable exception throwing
    if (!con.f.is_open())
    {
        throw std::runtime_error{"Failed to open '" + path + "'"};
    }

    // Read header.
    con.f.read((char *)&con.hdr, 244);
    if (strncmp(con.hdr.id, "IDST", 4) != 0)
    {
        fprintf(stderr, "Bad ID %.4s\n", (char*)&con.hdr.id);
    }
    result.name = std::string{con.hdr.name};

    // Load external texture data.
    if (con.hdr.numtextures == 0)
    {
        auto t = load_mdl(path.substr(0, path.length() - 4) + "t.mdl");
        for (auto const &t : t.textures)
            result.textures.push_back(t);
    }


    // Read Textures.
    for (uint32_t i = 0; i < con.hdr.numtextures; ++i)
    {
        result.textures.push_back(_load_texture(con, i));
    }

    // Read bodyparts hierarchy.
    _load_bodyparts(con, result);

    con.f.close();

    return result;
}
