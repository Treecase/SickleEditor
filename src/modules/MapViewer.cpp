/**
 * MapViewer.cpp - Map viewer module.
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

#include "MapViewer.hpp"
#include "../ui_helpers.hpp"
#include "../fgd/fgd.hpp"

#include <imgui.h>


/* ===[ MapViewer::GLBrush ]=== */
MapViewer::GLBrush::GLBrush(
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

/** Transform map Brush to GL brush. */
MapViewer::GLBrush *MapViewer::_brush2gl(
    MAP::Brush const &brush, MAP::TextureManager &textures)
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


/* ===[ MapViewer ]=== */
MapViewer::MapViewer(Config &cfg)
:   Base3DViewer{
        cfg, "Map Viewer", false, false,
        GLUtil::Program{{
            GLUtil::shader_from_file(
                "shaders/map.vert", GL_VERTEX_SHADER),
            GLUtil::shader_from_file(
                "shaders/map.frag", GL_FRAGMENT_SHADER)},
            "MapShader"},
        FreeCam{},
        false,
        2.0f
    }
,   _map{}
,   _brushes{}
,   _selected{""}
,   _transform{
        {0.0f, 0.0f, 0.0f},
        {glm::radians(-90.0f), 0.0f, 0.0f},
        {0.005f, 0.005f, 0.005f}}
{
}

void MapViewer::drawUI()
{
    if (!ui_visible)
        return;

    if (ImGui::Begin(title.c_str(), &ui_visible))
    {
        ImGui::TextUnformatted((
            "Map: "
            + (_selected.empty()? "<none>" : _selected.filename().string())
            ).c_str());
        if (ImGui::CollapsingHeader("Map Transform"))
            _transform.imgui();
        Base3DViewer::drawUI();
        ImGui::Separator();
        if (ImGui::BeginChild("MapTree"))
        {
            if (ImGui::TreeNode("valve/maps"))
            {
                if (ImGui::DirectoryTree(
                    _cfg.maps_dir,
                    &_selected,
                    [](std::filesystem::path const &p){
                        return p.extension() == ".map";
                    }))
                {
                    _loadSelectedMap();
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void MapViewer::drawGL(float deltaT)
{
    if (_selected.empty())
        return;

    Base3DViewer::drawGL(deltaT);

    // Setup projection matrix.
    auto const projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        *_cfg.window_width / (float)*_cfg.window_height,
        0.1f, 1000.0f);

    auto const modelMatrix = _transform.getMatrix();

    // Draw models.
    _shader.use();
    glActiveTexture(GL_TEXTURE0);
    _shader.setUniformS("view", _camera.getViewMatrix());
    _shader.setUniformS("projection", projectionMatrix);
    _shader.setUniformS("tex", 0);
    _shader.setUniformS("model", modelMatrix);

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


void MapViewer::_loadSelectedMap()
{
    auto fgd = FGD::load(_cfg.game_def.string());
    _map = MAP::load_map(_selected.string());
    _loadMap();
}

void MapViewer::_loadMap()
{
    MAP::Entity worldspawn;
    for (auto const &e : _map.entities)
        if (e.properties.at("classname") == "worldspawn")
        {
            worldspawn = e;
            break;
        }

    auto const &wad = WAD::load(worldspawn.properties.at("wad"));
    MAP::TextureManager textures{wad};

    _brushes.clear();
    for (auto const &b : worldspawn.brushes)
        _brushes.emplace_back(_brush2gl(b, textures));
}
