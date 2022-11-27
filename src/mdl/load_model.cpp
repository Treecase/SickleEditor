/**
 * load_model.cpp - Load .mdl files.
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
#include <cassert> // temp


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


/** Class to load MDLs. */
class MDLLoader
{
private:
    MDL::Model _result;
    std::ifstream _f;
    M_Header _hdr;


    /** Load BodyParts from the MDL. */
    void _loadBodyparts()
    {
        for (uint32_t i = 0; i < _hdr.numbodyparts; ++i)
        {
            M_BodyPart bodypart{};
            _f.seekg(_hdr.bodypartindex + 76 * i);
            _f.read(bodypart.name, 64);
            _f.read((char*)&bodypart.nummodels, 4);
            _f.read((char*)&bodypart.base, 4);
            _f.read((char*)&bodypart.modelindex, 4);
            _result.bodyparts.push_back({bodypart.name, {}});
            _loadBodypartModels(&bodypart, &_result.bodyparts[i]);
        }
    }

    /** Load a BodyPart's Models from the MDL. */
    void _loadBodypartModels(M_BodyPart *mbodypart, MDL::BodyPart *bodypart)
    {
        for (uint32_t i = 0; i < mbodypart->nummodels; ++i)
        {
            M_Model model{};
            _f.seekg(mbodypart->modelindex + 112 * i);
            _f.read(model.name, 64);
            _f.read((char*)&model.type, 4);
            _f.read((char*)&model.boundingradius, 4);
            _f.read((char*)&model.nummesh, 4);
            _f.read((char*)&model.meshindex, 4);
            _f.read((char*)&model.numverts, 4);
            _f.read((char*)&model.vertinfoindex, 4);
            _f.read((char*)&model.vertindex, 4);
            _f.read((char*)&model.numnorms, 4);
            _f.read((char*)&model.norminfoindex, 4);
            _f.read((char*)&model.normindex, 4);
            _f.read((char*)&model.numgroups, 4);
            _f.read((char*)&model.groupindex, 4);
            bodypart->models.push_back({model.name, {}, {}});
            _loadModelVertices(&model, &bodypart->models[i]);
            _loadModelMeshes(&model, &bodypart->models[i]);
        }
    }

    /** Load a Model's vertices from the MDL. */
    void _loadModelVertices(M_Model *mmodel, MDL::MDLModel *model)
    {
        _f.seekg(mmodel->vertindex);
        for (uint32_t i = 0; i < mmodel->numverts; ++i)
        {
            model->vertices.push_back({0.0f, 0.0f, 0.0f});
            auto &vertex = model->vertices.at(i);
            _f.read((char*)&vertex.x, 4);
            _f.read((char*)&vertex.y, 4);
            _f.read((char*)&vertex.z, 4);
        }
    }

    /** Load a Model's Meshes from the MDL. */
    void _loadModelMeshes(M_Model *mmodel, MDL::MDLModel *model)
    {
        for (uint32_t i = 0; i < mmodel->nummesh; ++i)
        {
            M_Mesh mesh;
            _f.seekg(mmodel->meshindex + 20 * i);
            _f.read((char*)&mesh.numtris, 4);
            _f.read((char*)&mesh.triindex, 4);
            _f.read((char*)&mesh.skinref, 4);
            _f.read((char*)&mesh.numnorms, 4);
            _f.read((char*)&mesh.normindex, 4);
            model->meshes.push_back({});
            _loadMeshTricmds(&mesh, &model->meshes[i]);
            model->meshes[i].skinref = mesh.skinref;
        }
    }

    /** Load a Mesh's tricmds from the MDL. */
    void _loadMeshTricmds(M_Mesh *mmesh, MDL::Mesh *mesh)
    {
        struct M_Vertex
        {
            uint16_t modelposition;
            uint16_t modellight;
            uint16_t uv_s, uv_t;
        };

        int16_t numverts;
        _f.seekg(mmesh->triindex);
        _f.read((char*)&numverts, 2);
        int16_t n = numverts < 0? -numverts : numverts;

        while (n != 0)
        {
            MDL::Tricmd tri;
            for (int16_t j = 0; j < n; ++j)
            {
                M_Vertex vertex;
                _f.read((char*)&vertex.modelposition, 2);
                _f.read((char*)&vertex.modellight, 2);
                _f.read((char*)&vertex.uv_s, 2);
                _f.read((char*)&vertex.uv_t, 2);
                tri.vertices.push_back({
                    vertex.modelposition,
                    vertex.modellight,
                    vertex.uv_s, vertex.uv_t});
            }
            tri.mode = numverts < 0;
            mesh->tricmds.push_back(tri);

            _f.read((char*)&numverts, 2);
            n = numverts < 0? -numverts : numverts;
        }
    }

    /** Load all the textures in the MDL. */
    void _loadTextures()
    {
        for (uint32_t i = 0; i < _hdr.numtextures; ++i)
        {
            _f.seekg(_hdr.textureindex + 80 * i);
            _result.textures.push_back(_loadTextureDirect(_f));
        }
        // Read skinrefs
        _f.seekg(_hdr.skinindex);
        for (uint32_t i = 0; i < _hdr.numskinref; ++i)
        {
            uint16_t s;
            _f.read((char*)&s, 2);
            _result.skinref.push_back(s);
        }
    }

    /** Load textures from external MDL. */
    void _loadExternalTextures(std::string const &path)
    {
        std::ifstream tf{path, std::ios::in | std::ios::binary};
        if (!tf.is_open())
            throw std::runtime_error{"Failed to open '" + path + "'"};
        // Read header.
        auto thdr = _loadHeaderDirect(tf);
        // Read textures into _result.
        for (uint32_t i = 0; i < thdr.numtextures; ++i)
        {
            tf.seekg(thdr.textureindex + 80 * i);
            _result.textures.push_back(_loadTextureDirect(tf));
        }
        // Read skinrefs
        tf.seekg(thdr.skinindex);
        for (uint32_t i = 0; i < thdr.numskinref; ++i)
        {
            uint16_t s;
            tf.read((char*)&s, 2);
            _result.skinref.push_back(s);
        }
        tf.close();
    }

    /** Load a texture. */
    MDL::Texture _loadTextureDirect(std::ifstream &f)
    {
        MDL::Texture result{};
        M_Texture tex;
        f.read((char *)&tex.name, 64);
        f.read((char *)&tex.flags, 4);
        f.read((char *)&tex.width, 4);
        f.read((char *)&tex.height, 4);
        f.read((char *)&tex.index, 4);

        result.name = std::string{tex.name};
        result.w = tex.width;
        result.h = tex.height;
        auto const buf = new char[tex.width * tex.height];
        f.seekg(tex.index);
        f.read(buf, tex.width * tex.height);
        f.read((char *)result.palette.data(), 256 * 3);

        result.data.reserve(tex.width * tex.height);
        for (size_t i = 0; i < tex.width * tex.height; ++i)
            result.data.push_back(buf[i]);
        delete[] buf;

        return result;
    }

    /** Load MDL header. */
    M_Header _loadHeaderDirect(std::ifstream &f)
    {
        MDL::Texture result{};
        M_Header hdr;
        f.read((char*)hdr.id, 4);
        f.read((char*)&hdr.version, 4);
        f.read((char*)hdr.name, 64);
        f.read((char*)&hdr.length, 4);
        f.read((char*)hdr.eyePosition, 12);
        f.read((char*)hdr.min, 12);
        f.read((char*)hdr.max, 12);
        f.read((char*)hdr.bbmin, 12);
        f.read((char*)hdr.bbmax, 12);
        f.read((char*)&hdr.flags, 4);
        f.read((char*)&hdr.numbones, 4);
        f.read((char*)&hdr.boneindex, 4);
        f.read((char*)&hdr.numbonecontrollers, 4);
        f.read((char*)&hdr.bonecontrollerindex, 4);
        f.read((char*)&hdr.numhitboxes, 4);
        f.read((char*)&hdr.hitboxindex, 4);
        f.read((char*)&hdr.numseq, 4);
        f.read((char*)&hdr.seqindex, 4);
        f.read((char*)&hdr.numseqgroups, 4);
        f.read((char*)&hdr.seqgroupindex, 4);
        f.read((char*)&hdr.numtextures, 4);
        f.read((char*)&hdr.textureindex, 4);
        f.read((char*)&hdr.texturedataindex, 4);
        f.read((char*)&hdr.numskinref, 4);
        f.read((char*)&hdr.numskinfamilies, 4);
        f.read((char*)&hdr.skinindex, 4);
        f.read((char*)&hdr.numbodyparts, 4);
        f.read((char*)&hdr.bodypartindex, 4);
        f.read((char*)&hdr.numattachments, 4);
        f.read((char*)&hdr.attachmentindex, 4);
        f.read((char*)&hdr.soundtable, 4);
        f.read((char*)&hdr.soundindex, 4);
        f.read((char*)&hdr.soundgroups, 4);
        f.read((char*)&hdr.soundgroupindex, 4);
        f.read((char*)&hdr.numtransitions, 4);
        f.read((char*)&hdr.transitionindex, 4);
        return hdr;
    }

public:
    MDLLoader(std::string const &path)
    :   _result{}
    ,   _f{path, std::ios::in | std::ios::binary}
    ,   _hdr{}
    {
        _f.exceptions(std::ifstream::failbit); // temp, enable exception throwing
        if (!_f.is_open())
            throw std::runtime_error{"Failed to open '" + path + "'"};

        // Load header.
        _hdr = _loadHeaderDirect(_f);
        if (strncmp(_hdr.id, "IDST", 4) != 0)
            throw std::runtime_error{"Bad ID " + std::string{_hdr.id}};
        _result.name = std::string{_hdr.name};

        // Load textures.
        if (_hdr.numtextures == 0)
            _loadExternalTextures(path.substr(0, path.length() - 4) + "t.mdl");
        else
            _loadTextures();

        // Load bodyparts.
        _loadBodyparts();
    }

    ~MDLLoader()
    {
        _f.close();
    }

    /** Get the result. */
    MDL::Model getResult() const
    {
        return _result;
    }
};


MDL::Model MDL::load(std::string const &path)
{
    MDLLoader con{path};
    return con.getResult();
}
