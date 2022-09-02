/**
 * bsp2gl.cpp - Convert .bsp data into OpenGL objects.
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

#include "bsp2gl.hpp"
#include "../wad/lumps.hpp"

#include <algorithm>
#include <deque>

#include <cstring>

#define TEXTURE_SCALING 0.01f


namespace BSP {
    /** So VertexDef can be used in unordered_map. */
    bool operator==(VertexDef const &lhs, VertexDef const &rhs)
    {
        return (
            lhs.x == rhs.x
            && lhs.y == rhs.y
            && lhs.z == rhs.z
            && lhs.s == rhs.s
            && lhs.t == rhs.t
        );
    }
}

/** So BSPVertexDef can be used in unordered_map. */
template<>
struct std::hash<BSP::VertexDef>
{
    size_t operator()(BSP::VertexDef const &v) const noexcept
    {
        return (
            std::hash<GLfloat>{}(v.x)
            ^ std::hash<GLfloat>{}(v.y)
            ^ std::hash<GLfloat>{}(v.z)
            ^ std::hash<GLfloat>{}(v.s)
            ^ std::hash<GLfloat>{}(v.t)
        );
    }
};

/** Convert BSP paletted texture data to RGBA format. */
std::vector<uint8_t *> depalettize(WAD::TexLump const &lump)
{
    std::vector<uint8_t *> rgba_textures{};

    auto const textures = {lump.tex1, lump.tex2, lump.tex4, lump.tex8};
    for (auto const &tex : textures)
    {
        auto rgba = new uint8_t[tex.size() * 4];
        for (size_t i = 0, j = 0; i < tex.size(); ++i, j += 4)
        {
            auto color = lump.palette[tex[i]];
            rgba[j+0] = color[0];
            rgba[j+1] = color[1];
            rgba[j+2] = color[2];
            rgba[j+3] = 0xff;
        }
        rgba_textures.push_back(rgba);
    }
    return rgba_textures;
}


class BSP2GL_Context
{
private:
    std::unordered_map<BSP::VertexDef, size_t> _vertIdx{};
    std::vector<BSP::VertexDef> _vboData;
    std::vector<GLuint> _eboData;


public:
    BSP2GL_Context()
    :   _vertIdx{}
    ,   _vboData{}
    ,   _eboData{}
    {
    }


    GLuint addVertex(BSP::VertexDef const &v)
    {
        GLuint n = 0;
        try
        {
            n = _vertIdx.at(v);
        }
        catch (std::out_of_range const &)
        {
            n = _vertIdx[v] = _vboData.size();
            _vboData.push_back(v);
        }
        _eboData.push_back(n);
        return n;
    }

    GLuint newFace()
    {
        _eboData.push_back(-1);
        return -1;
    }

    auto getVBO() const {return _vboData;}
    auto getEBO() const {return _eboData;}
};


BSP::GLBSP BSP::bsp2gl(BSP const &bsp, std::string const &game_dir)
{
    struct GLMeshData
    {
        std::vector<GLuint> ebo;
    };
    struct GLModelData
    {
        glm::vec3 position;
        // `meshes` has the same length as `bsp.textures`, since each mesh is
        // associated with texture.
        std::vector<GLMeshData> meshes;
    };

    // The Context collects VertexDefs into our VBO. This step is needed since
    // the BSP's vertex list doesn't contain vertex UV data, which we need for
    // the VBO.
    BSP2GL_Context context{};

    std::vector<GLModelData> models{};
    models.reserve(bsp.models.size());

    // Iterate over models in the map.
    for (auto const &model : bsp.models)
    {
        GLModelData model_data{
            {model.origin[0], model.origin[1], model.origin[2]},
            std::vector<GLMeshData>{bsp.textures.size()}
        };

        // Depth-first descent of the BSP tree associated with the model,
        // building a list of leaf nodes.
        std::vector<Leaf> leaves{};
        std::deque<uint16_t> stack{(uint16_t)model.node_id[0]};
        while (!stack.empty())
        {
            auto node_idx = stack.back();
            stack.pop_back();
            auto const &node = bsp.nodes.at(node_idx);
            // If the highest-order bit is 0, it's a Node child.
            if ((node.front & 0x8000) == 0)
                stack.push_back(node.front);
            // If the highest-order bit is 1, it's a Leaf child.
            else if (node.front != 65535)
                leaves.push_back(bsp.leaves.at((uint16_t)~node.front));
            // Same for back child.
            if ((node.back & 0x8000) == 0)
                stack.push_back(node.back);
            else if (node.back != 65535)
                leaves.push_back(bsp.leaves.at((uint16_t)~node.back));
        }

        // Iterate over all the model's leaves, adding the vertices to the
        // Context and creating Mesh EBOs.
        for (auto const &leaf : leaves)
        {
            for (
                auto marksurface = leaf.marksurface;
                marksurface < leaf.marksurface + leaf.marksurface_num;
                ++marksurface)
            {
                auto face_idx = bsp.marksurfaces.at(marksurface);
                auto const &face = bsp.faces.at(face_idx);
                auto const &texinfo = bsp.texinfo[face.texinfo];
                auto tex_idx = texinfo.texture;
                auto const &texture = bsp.textures[tex_idx];
                auto svec = texinfo.sVector;
                auto tvec = texinfo.tVector;
                glm::vec3 sv{svec[0], svec[1], svec[2]};
                glm::vec3 tv{tvec[0], tvec[1], tvec[2]};
                auto &mesh = model_data.meshes.at(tex_idx);
                // Surfedges are sorted to be clockwise, but we render
                // counter-clockwise, so we must reverse the order.
                for (
                    size_t se = 0;
                    se < face.surfedge_num;
                    ++se)
                {
                    auto se_idx = face.surfedge_num - se - 1;
                    auto ledge = bsp.surfedges.at(face.surfedge + se_idx);
                    bool reversed = ledge < 0;
                    auto edge_idx = reversed? -ledge : ledge;
                    auto const &edge = bsp.edges.at(edge_idx);

                    auto a = reversed? edge.start : edge.end;
                    auto b = reversed? edge.end : edge.start;

                    auto const &av = bsp.vertices[a];
                    auto const &bv = bsp.vertices[b];

                    glm::vec3 apos{av.x, av.y, av.z};
                    glm::vec3 bpos{bv.x, bv.y, bv.z};

                    glm::vec2 auv{
                        (glm::dot(apos, sv) + texinfo.sDist) / texture.width,
                        (glm::dot(apos, tv) + texinfo.tDist) / texture.height};
                    glm::vec2 buv{
                        (glm::dot(bpos, sv) + texinfo.sDist) / texture.width,
                        (glm::dot(bpos, tv) + texinfo.tDist) / texture.height};

                    mesh.ebo.push_back(context.addVertex({av.x, av.y, av.z, auv.s, auv.t}));
                    mesh.ebo.push_back(context.addVertex({bv.x, bv.y, bv.z, buv.s, buv.t}));
                }
                mesh.ebo.push_back(context.newFace());
            }
        }
        models.push_back(model_data);
    }

    GLBSP out{};
    std::vector<VertexDef> vboData = context.getVBO();
    std::vector<GLuint> eboData{};

    // Now that we have all the vertices in the Context VBO, we can flatten our
    // models' separate EBOs into one big one, which we'll actually send to GPU.
    auto const textures = getTextures(bsp, game_dir);
    for (auto const &model : models)
    {
        GLModel glmodel{};
        for (size_t i = 0; i < model.meshes.size(); ++i)
        {
            auto const &mesh = model.meshes[i];
            glmodel.meshes.push_back({
                textures[i],
                (GLsizei)mesh.ebo.size(),
                (void *)(eboData.size() * sizeof(GLuint))
            });
            eboData.insert(eboData.end(), mesh.ebo.cbegin(), mesh.ebo.cend());
        }
        out.models.push_back(glmodel);
    }

    out.vao.reset(new GLUtil::VertexArray{"mapVAO"});
    out.vao->bind();

    out.vbo.reset(new GLUtil::Buffer{GL_ARRAY_BUFFER, "mapVBO"});
    out.vbo->bind();
    out.vbo->buffer(GL_STATIC_DRAW, vboData);

    out.ebo.reset(new GLUtil::Buffer{GL_ELEMENT_ARRAY_BUFFER, "mapEBO"});
    out.ebo->bind();
    out.ebo->buffer(GL_STATIC_DRAW, eboData);

    out.vao->enableVertexAttribArray(
        0, 3, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, x));
    out.vao->enableVertexAttribArray(
        1, 2, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, s));

    out.ebo->unbind();
    out.vbo->unbind();
    out.vao->unbind();

    return out;
}

static std::vector<WAD::WAD> BSP_wads;

std::vector<GLUtil::Texture> BSP::getTextures(
    BSP const &bsp, std::string const &game_dir)
{
    if (BSP_wads.empty())
    {
        BSP_wads.push_back(WAD::load(game_dir + "/valve/halflife.wad"));
        BSP_wads.push_back(WAD::load(game_dir + "/valve/liquids.wad"));
        BSP_wads.push_back(WAD::load(game_dir + "/valve/xeno.wad"));
        BSP_wads.push_back(WAD::load(game_dir + "/valve/decals.wad"));
        BSP_wads.push_back(WAD::load(game_dir + "/valve/spraypaint.wad"));
    }

    std::vector<GLUtil::Texture> out{};
    for (auto const &bsptex : bsp.textures)
    {
        WAD::TexLump tex{};
        // TODO: Use a map instead of looping through everything
        for (auto const &wad : BSP_wads)
        {
            for (auto const &lump : wad.directory)
            {
                if (lump.type != 0x43)
                    continue;

                // Just grab the name real quick...
                char name[16];
                memcpy(name, lump.data.data(), 16);

                if (strncmp(name, bsptex.name.c_str(), 16) == 0)
                {
                    // Only read the whole lump if it's the one we need.
                    tex = WAD::readTexLump(lump);
                    goto done_search;
                }
            }
        }
        std::cerr << "Failed to find texture '" + bsptex.name + "'\n";
done_search:

        out.emplace_back(GL_TEXTURE_2D, tex.name);
        auto &t = out[out.size() - 1];

        t.bind();
        t.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        t.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        t.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        t.setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        t.setParameter(GL_TEXTURE_MAX_LEVEL, 3);

        auto mipmaps = depalettize(tex);
        for (size_t i = 0; i < mipmaps.size(); ++i)
        {
            GLsizei scale = pow(2, i);
            glTexImage2D(t.type(), i, GL_RGBA, tex.width/scale, bsptex.height/scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmaps[i]);
            delete[] mipmaps[i];
        }
        t.unbind();
    }
    return out;
}
