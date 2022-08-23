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


GLBSP bsp2gl(BSP::BSP const &bsp)
{
    GLBSP out{};

    for (auto const &v : bsp.vertices)
        out.vertices.push_back({v.x, v.y, v.z, 0.0f,0.0f});

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
            for (
                auto se = face.surfedge;
                se < face.surfedge + face.surfedge_num;
                ++se)
            {
                auto ledge = bsp.surfedges.at(se);
                bool reversed = ledge < 0;
                auto edge_idx = reversed? -ledge : ledge;
                auto const &edge = bsp.edges.at(edge_idx);

                auto a = reversed? edge.end : edge.start;
                auto b = reversed? edge.start : edge.end;

                out.indices.push_back((GLuint)a);
                out.indices.push_back((GLuint)b);
            }
            out.indices.push_back(-1);
        }
    }

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
