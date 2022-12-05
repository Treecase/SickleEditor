/**
 * BSPViewer.cpp - Map viewer module.
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

#include "BSPViewer.hpp"
#include "utils/ui_helpers.hpp"

#include <imgui.h>


BSPViewer::BSPViewer(Config &cfg)
:   Base3DViewer{cfg, "BSP Viewer", false, false,
        GLUtil::Program{{
            GLUtil::shader_from_file("shaders/map.vert", GL_VERTEX_SHADER),
            GLUtil::shader_from_file("shaders/map.frag", GL_FRAGMENT_SHADER)},
            "BSPShader"},
        FreeCam{},
        false,
        2.0f
    }
,   _map{}
,   _glbsp{}
,   _selected{""}
,   _transform{
        {0.0f, 0.0f, 0.0f},
        {glm::radians(-90.0f), 0.0f, 0.0f},
        {0.005f, 0.005f, 0.005f}}
{
}

void BSPViewer::drawUI()
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
            ImGui::Transform(&_transform);
        Base3DViewer::drawUI();
        ImGui::Separator();
        if (ImGui::BeginChild("MapTree"))
        {
            if (ImGui::TreeNode("valve/maps"))
            {
                if (ImGui::DirectoryTree(
                    _cfg.game_dir.string() + "/valve/maps",
                    &_selected,
                    [](std::filesystem::path const &p){
                        return p.extension() == ".bsp";
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

void BSPViewer::drawGL(float deltaT)
{
    if (_selected.empty())
        return;

    Base3DViewer::drawGL(deltaT);

    // Setup projection matrix.
    auto const projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        *_cfg.window_width / (float)*_cfg.window_height,
        0.1f, 1000.0f);

    auto const &modelMatrix = _transform.getMatrix();

    // Draw models.
    _shader.use();
    glActiveTexture(GL_TEXTURE0);
    _shader.setUniformS("view", _camera.getViewMatrix());
    _shader.setUniformS("projection", projectionMatrix);
    _shader.setUniformS("tex", 0);
    _glbsp.render();
}


void BSPViewer::_loadSelectedMap()
{
    _map = BSP::load(_selected.string());
    _glbsp = BSP::GLBSP{_map, _cfg.game_dir.string()};
}
