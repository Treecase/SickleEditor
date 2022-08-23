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

#define TEXTURE_SCALING 0.01f


/** So BSPVertexDef can be used in unordered_map. */
bool operator==(BSPVertexDef const &lhs, BSPVertexDef const &rhs)
{
    return (
        lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.z == rhs.z
        && lhs.s == rhs.s
        && lhs.t == rhs.t
    );
}

/** So BSPVertexDef can be used in unordered_map. */
template<>
struct std::hash<BSPVertexDef>
{
    size_t operator()(BSPVertexDef const &v) const noexcept
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


class BSP2GL_Context
{
private:
    std::unordered_map<BSPVertexDef, size_t> _vertIdx{};
    std::vector<BSPVertexDef> _vboData;
    std::vector<GLuint> _eboData;


public:
    BSP2GL_Context()
    :   _vertIdx{}
    ,   _vboData{}
    ,   _eboData{}
    {
    }


    void addVertex(BSPVertexDef const &v)
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
    }

    void newFace()
    {
        _eboData.push_back(-1);
    }

    auto getVBO() const {return _vboData;}
    auto getEBO() const {return _eboData;}
};


GLBSP bsp2gl(BSP::BSP const &bsp)
{
    BSP2GL_Context context{};

    // Descend the BSP tree.
    for (auto const &leaf : bsp.leaves)
    {
        if (leaf.type != -1)
            continue;
        for (
            auto marksurface = leaf.marksurface;
            marksurface < leaf.marksurface + leaf.marksurface_num;
            ++marksurface)
        {
            auto face_idx = bsp.marksurfaces.at(marksurface);
            auto const face = bsp.faces.at(face_idx);
            auto texinfo = bsp.texinfo[face.texinfo];
            auto svec = texinfo.sVector;
            auto tvec = texinfo.tVector;
            glm::vec3 sv{svec[0], svec[1], svec[2]};
            glm::vec3 tv{tvec[0], tvec[1], tvec[2]};
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

                auto av = bsp.vertices[a];
                auto bv = bsp.vertices[b];

                glm::vec3 apos{av.x, av.y, av.z};
                glm::vec3 bpos{bv.x, bv.y, bv.z};

                glm::vec2 auv{
                    glm::dot(apos, sv) + texinfo.sDist,
                    glm::dot(apos, tv) + texinfo.tDist};
                glm::vec2 buv{
                    glm::dot(bpos, sv) + texinfo.sDist,
                    glm::dot(bpos, tv) + texinfo.tDist};
                auv *= TEXTURE_SCALING;
                buv *= TEXTURE_SCALING;

                context.addVertex({av.x, av.y, av.z, auv.s, auv.t});
                context.addVertex({bv.x, bv.y, bv.z, buv.s, buv.t});
            }
            context.newFace();
        }
    }

    GLBSP out{};
    out.vertices = context.getVBO();
    out.indices = context.getEBO();

    out.vao.reset(new GLUtil::VertexArray{"mapVAO"});
    out.vao->bind();

    out.vbo.reset(new GLUtil::Buffer{GL_ARRAY_BUFFER, "mapVBO"});
    out.vbo->bind();
    out.vbo->buffer(GL_STATIC_DRAW, out.vertices);

    out.ebo.reset(new GLUtil::Buffer{GL_ELEMENT_ARRAY_BUFFER, "mapEBO"});
    out.ebo->bind();
    out.ebo->buffer(GL_STATIC_DRAW, out.indices);

    out.vao->enableVertexAttribArray(
        0, 3, GL_FLOAT, sizeof(BSPVertexDef), offsetof(BSPVertexDef, x));
    out.vao->enableVertexAttribArray(
        1, 2, GL_FLOAT, sizeof(BSPVertexDef), offsetof(BSPVertexDef, s));

    out.ebo->unbind();
    out.vbo->unbind();
    out.vao->unbind();

    return out;
}

GLUtil::Texture getBSPTextures(BSP::BSP const &bsp)
{
    auto bsptex = bsp.textures[0];
    GLUtil::Texture t{GL_TEXTURE_2D, "bspTexture"};
    t.bind();
    t.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    t.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    t.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    t.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    t.setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    t.setParameter(GL_TEXTURE_MAX_LEVEL, 3);
    glTexImage2D(
        t.type(), 0, GL_RED, bsptex.width, bsptex.height, 0, GL_RED,
        GL_UNSIGNED_BYTE, bsptex.tex1.get());
    glTexImage2D(
        t.type(), 1, GL_RED, bsptex.width/2, bsptex.height/2, 0, GL_RED,
        GL_UNSIGNED_BYTE, bsptex.tex2.get());
    glTexImage2D(
        t.type(), 2, GL_RED, bsptex.width/4, bsptex.height/4, 0, GL_RED,
        GL_UNSIGNED_BYTE, bsptex.tex4.get());
    glTexImage2D(
        t.type(), 3, GL_RED, bsptex.width/8, bsptex.height/8, 0, GL_RED,
        GL_UNSIGNED_BYTE, bsptex.tex8.get());
    t.unbind();
    return t;
}
