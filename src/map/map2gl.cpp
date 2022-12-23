/**
 * map2gl.cpp - Convert .map data into OpenGL objects.
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

#include "map2gl.hpp"

#include "../convexhull/convexhull.hpp"

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <array>
#include <set>
#include <unordered_set>
#include <vector>

#include <cstring>


/** A Plane in 3D space. */
struct Plane
{
    // 3 points used to define the plane.
    std::array<glm::vec3, 3> points;
    // Plane normal.
    glm::vec3 normal;
    // Coefficients for the general form plane equation (ax + by + cz + d = 0).
    float a;
    float b;
    float c;
    float d;

    Plane(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c)
    :   points{a, b, c}
    ,   normal{glm::normalize(glm::cross(c - a, b - a))}
    ,   a{normal.x}
    ,   b{normal.y}
    ,   c{normal.z}
    ,   d{-normal.x*a.x - normal.y*a.y - normal.z*a.z}
    {
        assert(containsPoint(a));
        assert(containsPoint(b));
        assert(containsPoint(c));
    }

    Plane(MAP::Plane const &p)
    :   Plane{
        {p.a[0], p.a[1], p.a[2]},
        {p.b[0], p.b[1], p.b[2]},
        {p.c[0], p.c[1], p.c[2]}}
    {
    }

    /** Check if `point` lies on the plane. */
    bool containsPoint(glm::vec3 const &point) const
    {
        static float constexpr EPSILON = 0.001f;
        return glm::epsilonEqual(
            a*point.x + b*point.y + c*point.z + d, 0.0f, EPSILON);
    }
};

#if 0 // temporarily in hv_enumeration.hpp
/** std::hash template so glm::vec3 can be used in std::set and friends. */
template<>
struct std::hash<glm::vec3>
{
    std::size_t operator()(glm::vec3 const &vec) const noexcept
    {
        std::hash<float> const hashf{};
        return hashf(vec.x) ^ hashf(vec.y) ^ hashf(vec.z);
    }
};
#endif

/** Implements std::less interface to sort glm::vec3s counterclockwise. */
struct VectorLessCounterClockwise
{
    // Precalculated center of points to be compared.
    glm::vec3 const center;
    // Plane to compare in.
    Plane const plane;
    // Plane's S and T axes.
    glm::vec3 const s_axisN, t_axisN;

    VectorLessCounterClockwise(glm::vec3 const &center, Plane const &plane)
    :   center{center}
    ,   plane{plane}
    ,   s_axisN{glm::normalize(plane.points[1] - plane.points[0])}
    ,   t_axisN{glm::normalize(glm::cross(s_axisN, plane.normal))}
    {
    }
    /** Sorts vertices counterclockwise. */
    bool operator()(glm::vec3 const &a, glm::vec3 const &b) const
    {
        // Local space A and B vectors.
        auto const a_local = a - center;
        auto const b_local = b - center;

        // Vertices projected onto the plane.
        glm::vec2 const a_proj{
            glm::dot(a_local, s_axisN), glm::dot(a_local, t_axisN)};
        glm::vec2 const b_proj{
            glm::dot(b_local, s_axisN), glm::dot(b_local, t_axisN)};

        // Angle between the 0 vector and point
        auto a_theta = glm::atan(a_proj.y, a_proj.x);
        auto b_theta = glm::atan(b_proj.y, b_proj.x);

        // Don't want negative angles. (Doesn't really matter, but whatever.)
        if (a_theta < 0.0f) a_theta += glm::radians(360.0);
        if (b_theta < 0.0f) b_theta += glm::radians(360.0);

        // If the angles aren't equal, compare them.
        // If the angles are the same, use distance from center as tiebreaker.
        if (glm::epsilonNotEqual(a_theta, b_theta, glm::epsilon<float>()))
            return -a_theta < -b_theta;
        else
            return glm::length(a_proj) < glm::length(b_proj);
    }
};

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

/** Sort vertices counterclockwise. */
auto _sort_vertices_counterclockwise(
    std::unordered_set<glm::vec3> const &vertices, Plane const &plane)
{
    glm::vec3 center{0.0f};
    for (auto const &vertex : vertices)
        center += vertex;
    center /= vertices.size();
    std::set<glm::vec3, VectorLessCounterClockwise> sorted{
        vertices.cbegin(), vertices.cend(),
        VectorLessCounterClockwise{center, plane}};
    assert(sorted.size() == vertices.size());
    return sorted;
}

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

/** Create a Mesh from a Brush's Planes. */
std::vector<Mesh> mesh_from_planes(
    MAP::Brush const &brush, MAP::TextureManager &textures)
{
    // Convert Brush Planes to Planes.
    std::vector<::Plane> polygon{};
    for (auto const &p : brush.planes)
        polygon.emplace_back(p);

    std::vector<HalfPlane> halfplanes{};
    for (auto const &p : polygon)
        halfplanes.push_back(HalfPlane{-p.a, -p.b, -p.c, -p.d});
    auto vertices = vertex_enumeration(halfplanes);

    // Build meshes from the plane vertices.
    std::vector<Mesh> meshes{};
    for (auto const &p : brush.planes)
    {
        // Find points that lie on the plane.
        std::unordered_set<glm::vec3> plane_points{};
        ::Plane const pl{p};
        for (auto const &vertex : vertices)
            if (pl.containsPoint(vertex))
                plane_points.emplace(vertex);
        // Create the mesh and add the points to it.
        meshes.emplace_back();
        auto &mesh = meshes.back();
        mesh.tex = p.miptex;
        auto const &sorted = _sort_vertices_counterclockwise(plane_points, pl);
        for (auto const &point : sorted)
        {
            assert(pl.containsPoint(point));
            // TODO: texcoord rotation
            auto const s = glm::normalize(
                glm::vec3{p.offx[0], p.offx[1], p.offx[2]});
            auto const t = glm::normalize(
                glm::vec3{p.offy[0], p.offy[1], p.offy[2]});
            glm::vec2 const offset{p.offx[3], p.offy[3]};
            glm::vec2 const scale{p.scalex, p.scaley};
            auto const &texture = textures.at(p.miptex);
            mesh.vbo.insert(
                mesh.vbo.end(),
                {   point.x, point.y, point.z,
                    ((glm::dot(point, s) / scale.x) + offset.s)
                        / (float)texture.w,
                    ((glm::dot(point, t) / scale.y) + offset.t)
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
    MAP::Brush const &brush, TextureManager &textures)
{
    // Merge Plane mesh V/EBOs into Brush V/EBO.
    std::vector<GLPlane> planes{};
    std::vector<GLfloat> vbodata{};
    std::vector<GLuint> ebodata{};
    for (auto const &mesh : mesh_from_planes(brush, textures))
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
