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

#include <algorithm>

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
uint8_t *_depalettize(uint8_t *data, size_t pixels, BSP::Palette const &palette)
{
    auto rgba = new uint8_t[pixels * 4];
    for (size_t i = 0, j = 0; i < pixels; ++i, j += 4)
    {
        auto color = palette[data[i]];
        rgba[j+0] = color[0];
        rgba[j+1] = color[1];
        rgba[j+2] = color[2];
        rgba[j+3] = 0xff;
    }
    return rgba;
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


    void addVertex(BSP::VertexDef const &v)
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


BSP::GLBSP BSP::bsp2gl(BSP const &bsp, WAD::WAD const &wad)
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
        0, 3, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, x));
    out.vao->enableVertexAttribArray(
        1, 2, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, s));

    out.ebo->unbind();
    out.vbo->unbind();
    out.vao->unbind();

    return out;
}

std::vector<GLUtil::Texture> BSP::getTextures(BSP const &bsp, WAD::WAD const &wad)
{
    std::vector<GLUtil::Texture> out{};
    for (auto const &bsptex : bsp.textures)
    {
        Palette palette{};
        bool found = false;
        for (auto const &lump : wad.directory)
        {
            auto tmp = bsptex.name;
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](auto c){return std::toupper(c);});
            if (lump.name == tmp)
            {
                memcpy(palette.data(), lump.data.data()+16, 768);
                found = true;
                break;
            }
        }
        // assert(found);

        out.emplace_back(GL_TEXTURE_2D, bsptex.name);
        auto &t = out[out.size() - 1];

        t.bind();
        t.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        t.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        t.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        t.setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        t.setParameter(GL_TEXTURE_MAX_LEVEL, 3);

        auto tdat = _depalettize(
            bsptex.tex1.get(),
            bsptex.width * bsptex.height,
            palette);
        glTexImage2D(
            t.type(), 0, GL_RGBA, bsptex.width, bsptex.height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, tdat);
        delete[] tdat;

        tdat = _depalettize(
            bsptex.tex2.get(),
            (bsptex.width/2) * (bsptex.height/2),
            palette);
        glTexImage2D(
            t.type(), 1, GL_RGBA, bsptex.width/2, bsptex.height/2, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, tdat);
        delete[] tdat;

        tdat = _depalettize(
            bsptex.tex4.get(),
            (bsptex.width/4) * (bsptex.height/4),
            palette);
        glTexImage2D(
            t.type(), 2, GL_RGBA, bsptex.width/4, bsptex.height/4, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, tdat);
        delete[] tdat;

        tdat = _depalettize(
            bsptex.tex8.get(),
            (bsptex.width/8) * (bsptex.height/8),
            palette);
        glTexImage2D(
            t.type(), 3, GL_RGBA, bsptex.width/8, bsptex.height/8, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, tdat);
        delete[] tdat;
        t.unbind();
    }
    return out;
}
