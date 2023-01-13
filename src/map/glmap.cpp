/**
 * glmap.cpp - Convert .map data into OpenGL objects.
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

#include "map/glmap.hpp"

#include <array>
#include <vector>
#include <cstring>


// TODO: move this into glutil?
/**
 * A Mesh, containing a texture name, vertex data, and element buffer data.
 */
struct Mesh
{
    std::string tex;
    std::vector<GLfloat> vbo{};
    std::vector<GLuint> ebo{};
};

/** Convert paletted texture data to RGBA format. */
auto _texlump_depalettize(WAD::TexLump const &lump)
{
    std::array<std::vector<uint8_t>, 4> const textures{
        lump.tex1, lump.tex2, lump.tex4, lump.tex8};
    std::array<uint8_t *, 4> rgba_textures{};
    for (size_t i = 0; i < rgba_textures.size(); ++i)
    {
        auto rgba = rgba_textures[i] = new uint8_t[textures[i].size() * 4];
        for (size_t j = 0, k = 0; j < textures[i].size(); ++j, k += 4)
        {
            auto const &color = lump.palette[textures[i][j]];
            memcpy(rgba + k, color.data(), 3);
            rgba[k+3] = 0xff;
        }
    }
    return rgba_textures;
}

/** Create a Mesh from a VBrush. */
std::vector<Mesh> mesh_from_brush(
    MAP::Brush const &brush, MAP::TextureManager &textures)
{
    // Build meshes from the brush faces.
    std::vector<Mesh> meshes{};
    for (auto const &f : brush.planes)
    {
        // Create the mesh and add the points to it.
        auto &mesh = meshes.emplace_back();
        mesh.tex = f.miptex;

        auto const s = glm::normalize(glm::vec3{f.s.x, f.s.y, f.s.z});
        auto const t = glm::normalize(glm::vec3{f.t.x, f.t.y, f.t.z});
        auto const &texture = textures.at(f.miptex);

        for (auto const &point : f.vertices)
        {
            // TODO: texcoord rotation
            mesh.vbo.insert(
                mesh.vbo.end(),
                {   point.x, point.y, point.z,
                    ((glm::dot(point, s) / f.scale.s) + f.offsets.s)
                        / (float)texture.w,
                    ((glm::dot(point, t) / f.scale.t) + f.offsets.t)
                        / (float)texture.h
                });
            mesh.ebo.push_back(mesh.ebo.size());
        }
    }
    return meshes;
}


/* ===[ MapTexture ]=== */
MAP::MapTexture::MapTexture()
:   texture{nullptr}
,   w{0}
,   h{0}
{
}

MAP::MapTexture::MapTexture(WAD::TexLump const &texlump)
:   texture{new GLUtil::Texture{GL_TEXTURE_2D, texlump.name}}
,   w{(int)texlump.width}
,   h{(int)texlump.height}
{
    texture->bind();
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    texture->setParameter(GL_TEXTURE_MAX_LEVEL, 3);
    auto mipmaps = _texlump_depalettize(texlump);
    for (size_t i = 0; i < mipmaps.size(); ++i)
    {
        GLsizei scale = pow(2, i);
        glTexImage2D(
            texture->type(), i, GL_RGBA,
            texlump.width/scale, texlump.height/scale, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, mipmaps[i]);
        delete[] mipmaps[i];
    }
    texture->unbind();
}


/* ===[ GLBrush ]=== */
MAP::GLBrush::GLBrush(
    std::vector<GLPlane> const &planes, std::vector<GLfloat> const &vbodata,
    std::vector<GLuint> const &ebodata)
:   planes{planes}
,   vao{"BrushVAO"}
,   vbo{GL_ARRAY_BUFFER, "BrushVBO"}
,   ebo{GL_ELEMENT_ARRAY_BUFFER, "BrushEBO"}
{
    vao.bind();
    vbo.bind();
    vbo.buffer(GL_STATIC_DRAW, vbodata);
    ebo.bind();
    ebo.buffer(GL_STATIC_DRAW, ebodata);
    vao.enableVertexAttribArray(0, 3, GL_FLOAT, 5*sizeof(GLfloat), 0);
    vao.enableVertexAttribArray(
        1, 2, GL_FLOAT, 5*sizeof(GLfloat), 3*sizeof(GLfloat));
    ebo.unbind();
    vbo.unbind();
    vao.unbind();
}

MAP::GLBrush *MAP::GLBrush::new_from_brush(
    Brush const &brush, TextureManager &textures)
{
    // Merge Plane mesh V/EBOs into Brush V/EBO.
    std::vector<GLPlane> planes{};
    std::vector<GLfloat> vbodata{};
    std::vector<GLuint> ebodata{};
    for (auto const &mesh : mesh_from_brush(brush, textures))
    {
        planes.push_back({
            *textures.at(mesh.tex).texture,
            (GLsizei)mesh.ebo.size(),
            (void *)(ebodata.size() * sizeof(GLuint))});
        size_t const index = vbodata.size() / 5;
        for (auto const &idx : mesh.ebo)
            ebodata.push_back(index + idx);
        vbodata.insert(vbodata.end(), mesh.vbo.cbegin(), mesh.vbo.cend());
    }
    return new GLBrush{planes, vbodata, ebodata};
}


/* ===[ GLMap ]=== */
MAP::GLMap::GLMap()
:   _brushes{}
{
}

MAP::GLMap::GLMap(Map const &map)
:   _brushes{}
{
    Entity worldspawn;
    for (auto const &e : map.entities)
        if (e.properties.at("classname") == "worldspawn")
        {
            worldspawn = e;
            break;
        }

    auto const &wadpaths = worldspawn.properties.at("wad");
    TextureManager textures{};
    for (size_t i = 0, j = 0; j != std::string::npos; i = j + 1)
    {
        j = wadpaths.find(';', i);
        auto const n = (j == std::string::npos? j : j - i);
        auto const &wadpath = wadpaths.substr(i, n);
        textures.add_wad(WAD::load(wadpath));
    }

    for (auto const &b : worldspawn.brushes)
        _brushes.emplace_back(GLBrush::new_from_brush(b, textures));
}

void MAP::GLMap::render()
{
    for (auto const &brush : _brushes)
    {
        brush->vao.bind();
        brush->ebo.bind();
        for (auto const &plane : brush->planes)
        {
            plane.texture.bind();
            glDrawElements(
                GL_TRIANGLE_FAN, plane.count, GL_UNSIGNED_INT, plane.indices);
        }
    }
}
