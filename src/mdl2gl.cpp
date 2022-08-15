/**
 * mdl2gl.cpp - Convert loaded MDL data into OpenGL objects.
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

#include "mdl2gl.hpp"


template<typename T> T min(T a, T b) {return a < b? a : b;}

#include <unordered_map>


// Used by model2mesh, so MDL::Vertex can be used in unordered_map.
namespace MDL
{
    bool operator==(MDL::Vertex const &lhs, MDL::Vertex const &rhs)
    {
        return (
            lhs.position_index == rhs.position_index
            // && lhs.light_index == rhs.light_index
            && lhs.uv_s == rhs.uv_s
            && lhs.uv_t == rhs.uv_t);
    }
}
// Used by model2mesh, so MDL::Vertex can be used in unordered_map.
template<>
struct std::hash<MDL::Vertex>
{
    size_t operator()(MDL::Vertex const &v) const noexcept
    {
        return (
            std::hash<int>{}(v.position_index)
            // ^ std::hash<int>{}(v.light_index)
            ^ std::hash<int>{}(v.uv_s)
            ^ std::hash<int>{}(v.uv_t));
    }
};

GLUtil::Texture texture2GLTexture(MDL::Texture const &texture)
{
    // Calculate resized power-of-two dimensions.
    int resized_w = texture.w > 256? 256 : 1;
    int resized_h = texture.h > 256? 256 : 1;
    for (; resized_w < texture.w && resized_w < 256; resized_w *= 2)
        ;
    for (; resized_h < texture.h && resized_h < 256; resized_h *= 2)
        ;

    // Generate the resized texture.
    auto unpaletted = new GLubyte[resized_w * resized_h * 4];
    for (int y = 0; y < resized_h; ++y)
    {
        for (int x = 0; x < resized_w; ++x)
        {
            // TODO: clean this up
            // The x,y coordinates of the samples.
            int x1 = min(((x+0.25)/(float)resized_w)*texture.w, texture.w-1.0);
            int x2 = min(((x+0.75)/(float)resized_w)*texture.w, texture.w-1.0);
            int y1 = min(((y+0.25)/(float)resized_h)*texture.h, texture.h-1.0);
            int y2 = min(((y+0.75)/(float)resized_h)*texture.h, texture.h-1.0);
            // Palette indices of the sampled pixels.
            uint8_t indices[4] = {
                texture.data.at(y1*texture.w + x1),
                texture.data.at(y1*texture.w + x2),
                texture.data.at(y2*texture.w + x1),
                texture.data.at(y2*texture.w + x2)
            };
            // RGB triples of the sampled pixels.
            std::array<uint8_t, 3> samples[4] = {
                texture.palette.at(indices[0]),
                texture.palette.at(indices[1]),
                texture.palette.at(indices[2]),
                texture.palette.at(indices[3])
            };
            // Averaged RGB value of the samples.
            int averages[3] = {
                (samples[0][0]+samples[1][0]+samples[2][0]+samples[3][0])/4,
                (samples[0][1]+samples[1][1]+samples[2][1]+samples[3][1])/4,
                (samples[0][2]+samples[1][2]+samples[2][2]+samples[3][2])/4,
            };
            int offset = (y * resized_w + x) * 4;
            unpaletted[offset+0] = averages[0];
            unpaletted[offset+1] = averages[1];
            unpaletted[offset+2] = averages[2];
            unpaletted[offset+3] = 0xFF;
        }
    }

    // Create the GL texture.
    GLUtil::Texture t{GL_TEXTURE_2D, texture.name};
    t.bind();
    t.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        t.type(), 0, GL_RGBA, resized_w, resized_h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, unpaletted);
    delete[] unpaletted;
    t.unbind();
    return t;
}

/**
 * Add a vertex to `mesh`. If `v` is already in `verts`, the stored index from
 * there is used, otherwise it is pushed to `mesh`'s vertices vector.
 */
void addVertex(
    MeshDef &mesh, std::unordered_map<MDL::Vertex, GLuint> &verts,
    MDL::MDLModel const &model, MDL::Vertex const &v)
{
    GLuint n = 0;
    try
    {
        n = verts.at(v);
    }
    catch (std::out_of_range const &)
    {
        auto p = model.vertices.at(v.position_index);
        n = verts[v] = mesh.vertices.size();
        mesh.vertices.push_back({
            p.x, p.y, p.z,
            v.uv_s, v.uv_t,
            1.0f, 1.0f, 1.0f});
    }
    mesh.indices.push_back(n);
}

// Convert MDL model vertex data to a GL-friendly format.
MeshDef model2mesh(MDL::MDLModel const &model)
{
    MeshDef out{};
    std::unordered_map<MDL::Vertex, GLuint> vert_idxs{};

    // EBO data
    for (auto const &mesh : model.meshes)
    {
        for (auto const &tricmd : mesh.tricmds)
        {
            if (tricmd.mode)
            {
                // Triangle fan.
                for (size_t i = 1; i < tricmd.vertices.size()-1; i++)
                {
                    auto const &a = tricmd.vertices.at(0);
                    auto const &b = tricmd.vertices.at(i+1);
                    auto const &c = tricmd.vertices.at(i);
                    addVertex(out, vert_idxs, model, a);
                    addVertex(out, vert_idxs, model, b);
                    addVertex(out, vert_idxs, model, c);
                }
            }
            else
            {
                // Triangle strip.
                for (size_t i = 0; i < tricmd.vertices.size()-2; ++i)
                {
                    auto const &a = tricmd.vertices.at(i);
                    auto const &b = tricmd.vertices.at(i % 2 == 0? i+2 : i+1);
                    auto const &c = tricmd.vertices.at(i % 2 == 0? i+1 : i+2);
                    addVertex(out, vert_idxs, model, a);
                    addVertex(out, vert_idxs, model, b);
                    addVertex(out, vert_idxs, model, c);
                }
            }
        }
    }
    return out;
}

ModelDef model2vao(MDL::Model const &model)
{
    ModelDef out{};

    for (auto const &bodypart : model.bodyparts)
    {
        auto mesh = model2mesh(bodypart.models[0]);

        out.count.push_back(mesh.indices.size());
        out.indices.push_back((void*)out.meshData.indices.size());

        out.meshData.vertices.insert(
            out.meshData.vertices.end(),
            mesh.vertices.begin(), mesh.vertices.end());
        out.meshData.indices.insert(
            out.meshData.indices.end(),
            mesh.indices.begin(), mesh.indices.end());
    }

    // Create the VAO.
    out.vao.reset(new GLUtil::VertexArray{model.name + "VAO"});
    out.vao->bind();

    // Buffer the vertex data.
    out.vbo.reset(new GLUtil::Buffer{GL_ARRAY_BUFFER, model.name + "VBO"});
    out.vbo->bind();
    out.vbo->buffer(GL_STATIC_DRAW, out.meshData.vertices);

    // Buffer the index data.
    out.ebo.reset(
        new GLUtil::Buffer{GL_ELEMENT_ARRAY_BUFFER, model.name + "EBO"});
    out.ebo->bind();
    out.ebo->buffer(GL_STATIC_DRAW, out.meshData.indices);

    // Populate vertex positions array.
    out.vao->enableVertexAttribArray(
        0, 3, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, x));
    // Populate vertex UV array.
    out.vao->enableVertexAttribArray(
        1, 2, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, s));
    // Populate vertex color array.
    out.vao->enableVertexAttribArray(
        2, 3, GL_FLOAT, sizeof(VertexDef), offsetof(VertexDef, r));

    out.ebo->unbind();
    out.vbo->unbind();
    out.vao->unbind();
    return out;
}
