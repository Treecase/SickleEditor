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

#include <unordered_map>


template<typename T> T min(T a, T b) {return a < b? a : b;}


/** Used by MDL2GL, so VertexDef can be used in unordered_map. */
bool operator==(VertexDef const &lhs, VertexDef const &rhs)
{
    return (
        lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.z == rhs.z
        && lhs.s == rhs.s
        && lhs.t == rhs.t
        && lhs.r == rhs.r
        && lhs.g == rhs.g
        && lhs.b == rhs.b
    );
}

/** Used by MDL2GL, so VertexDef can be used in unordered_map. */
template<>
struct std::hash<VertexDef>
{
    size_t operator()(VertexDef const &v) const noexcept
    {
        return (
            std::hash<GLfloat>{}(v.x)
            ^ std::hash<GLfloat>{}(v.y)
            ^ std::hash<GLfloat>{}(v.z)
            ^ std::hash<GLfloat>{}(v.s)
            ^ std::hash<GLfloat>{}(v.t)
            ^ std::hash<GLfloat>{}(v.r)
            ^ std::hash<GLfloat>{}(v.g)
            ^ std::hash<GLfloat>{}(v.b)
        );
    }
};

/** Converts MDL::Model data to GL ready format. */
class MDL2GL
{
private:
    /* VBO/EBO data. */
    // Vertex definitions. (position, uvs, etc.)
    std::vector<VertexDef> _vboData;
    // Reference the vertices in _vboData to define the actual triangles of the
    // model.
    std::vector<GLuint> _eboData;

    // Instead of having to search through _vboData directly to check if a
    // vertex already exists, this maps its index directly for quick lookup.
    std::unordered_map<VertexDef, GLuint> _vertIdx;

    /* glDrawElements data. */
    // Contains the number of vertices in each mesh.
    std::vector<GLsizei> _count;
    // Contains the offset into _eboData for the first vertex of each mesh.
    std::vector<void*> _indices;
    // Texture index for each mesh.
    std::vector<size_t> _textureIndices;

    MDL::Model const &_modelData;

    /**
     * If v refers to a vertex already in _vboData, the vertex is added by
     * pushing its index to _eboData. If v is not already in _vboData, then it
     * is added to _vboData, and that new index is pushed to _eboData.
     */
    void addVertex(
        std::vector<MDL::Vec3> const &vertices, MDL::Vertex const &v,
        MDL::Texture const &texture)
    {
        auto p = vertices.at(v.position_index);
        VertexDef vd{
            p.x, p.y, p.z,
            v.uv_s / (float)(texture.w-1), v.uv_t / (float)(texture.h-1),
            1.0f, 1.0f, 1.0f
        };
        GLuint n = 0;
        try
        {
            n = _vertIdx.at(vd);
        }
        catch (std::out_of_range const &)
        {
            n = _vertIdx[vd] = _vboData.size();
            _vboData.push_back(vd);
        }
        n = _vertIdx[vd] = _vboData.size();
        _vboData.push_back(vd);
        _eboData.push_back(n);
    }

    /** Add a Mesh's vertices to the GL data. */
    void _mesh(MDL::Mesh const &mesh, std::vector<MDL::Vec3> const &vertices)
    {
        auto texIdx = _modelData.skinref.at(mesh.skinref);
        auto texture = _modelData.textures.at(texIdx);
        _textureIndices.push_back(texIdx);
        size_t preCount = _eboData.size();
        _indices.push_back((void*)(preCount * sizeof(GLuint)));
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
                    addVertex(vertices, a, texture);
                    addVertex(vertices, b, texture);
                    addVertex(vertices, c, texture);
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
                    addVertex(vertices, a, texture);
                    addVertex(vertices, b, texture);
                    addVertex(vertices, c, texture);
                }
            }
        }
        _count.push_back(_eboData.size() - preCount);
    }

    /** Add all the meshes from an MDLModel to the GL data. */
    void _model(MDL::MDLModel const &model)
    {
        for (auto &mesh : model.meshes)
            _mesh(mesh, model.vertices);
    }

public:
    MDL2GL(MDL::Model const &model)
    :   _vboData{}
    ,   _eboData{}
    ,   _vertIdx{}
    ,   _count{}
    ,   _indices{}
    ,   _textureIndices{}
    ,   _modelData{model}
    {
        for (auto const &bodypart : model.bodyparts)
            for (auto const &mdlmodel : bodypart.models)
                _model(mdlmodel);
    }

    auto getVBO() const {return _vboData;}
    auto getEBO() const {return _eboData;}
    auto getCount() const {return _count;}
    auto getIndices() const {return _indices;}
    auto getTextureIndices() const {return _textureIndices;}
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

GLMDL model2vao(MDL::Model const &model)
{
    MDL2GL glmdl{model};
    GLMDL out{};

    out.count = glmdl.getCount();
    out.indices = glmdl.getIndices();
    out.texture = glmdl.getTextureIndices();

    // Create the VAO.
    out.vao.reset(new GLUtil::VertexArray{model.name + "VAO"});
    out.vao->bind();

    // Buffer the vertex data.
    out.vbo.reset(new GLUtil::Buffer{GL_ARRAY_BUFFER, model.name + "VBO"});
    out.vbo->bind();
    out.vbo->buffer(GL_STATIC_DRAW, glmdl.getVBO());

    // Buffer the index data.
    out.ebo.reset(
        new GLUtil::Buffer{GL_ELEMENT_ARRAY_BUFFER, model.name + "EBO"});
    out.ebo->bind();
    out.ebo->buffer(GL_STATIC_DRAW, glmdl.getEBO());

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
