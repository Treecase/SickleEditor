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

#include <vector>
#include <fstream>
#include <iostream> // temp

#include <cstdint>
#include <cstring>


typedef float vec3[3];

struct Header
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

struct Bone
{
    char name[32];
    int parent;
    int flags;
    int bonecontroller[6];
    float value[6];
    float scale[6];
};

struct BoneController
{
    int bone;
    int type;
    float start;
    float end;
    int rest;
    int index;
};

struct Hitbox
{
    // TODO
};

struct Seq
{
    // TODO
};

struct SeqGroup
{
    char label[32];
    char name[64];
    void *cache;
    int data;
};

struct M_Texture
{
    char name[64];
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    uint32_t index;
};

struct Skin
{
    // TODO
};

struct BodyPart
{
    char name[64];
    int nummodels;
    int base;
    int modelindex;
};

struct Attachment
{
    char name[32];
    int type;
    int bone;
    vec3 org;
    vec3 vectors[3];
};


/** Read a data section. */
template<typename T>
std::vector<T> read_data(std::ifstream &f, uint32_t count, uint32_t index)
{
    std::vector<T> result{count};
    f.seekg(index);
    for (uint32_t i = 0; i < count; ++i)
    {
        f.read((char *)&result[i], sizeof(T));
    }
    return result;
}


MDL::Texture _load_texture(std::istream &f, Header &hdr, uint32_t texture)
{
    MDL::Texture result{};

    M_Texture tex;
    f.seekg(hdr.textureindex + 80*texture);
    f.read((char *)&tex.name, 64);
    f.read((char *)&tex.flags, 4);
    f.read((char *)&tex.width, 4);
    f.read((char *)&tex.height, 4);
    f.read((char *)&tex.index, 4);

    result.name = std::string{tex.name};
    result.w = tex.width;
    result.h = tex.height;
    result.data = new uint8_t[tex.width * tex.height];
    f.seekg(tex.index);
    f.read((char *)result.data, tex.width * tex.height);
    f.read((char *)result.palette, 256 * 3);

    return result;
}


MDL::Model MDL::load_mdl(std::string const &path)
{
    MDL::Model result{};

    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
    {
        throw std::runtime_error{"Failed to open '" + path + "'"};
    }

    // Read header.
    Header header{};
    f.read((char *)&header, 244);
    if (strncmp(header.id, "IDST", 4) != 0)
    {
        fprintf(stderr, "Bad ID %.4s\n", (char*)&header.id);
    }
    result.name = std::string{header.name};

    for (uint32_t i = 0; i < header.numtextures; ++i)
    {
        result.textures.push_back(_load_texture(f, header, i));
    }

    f.close();

    /* ===[ Post Load ]=== */
    if (header.numtextures == 0)
    {
        auto texmdl = path.substr(0, path.length()-4) + "t.mdl";
        auto submdl = load_mdl(texmdl);
        result.textures.insert(result.textures.end(), submdl.textures.begin(), submdl.textures.end());
    }

    return result;
}
