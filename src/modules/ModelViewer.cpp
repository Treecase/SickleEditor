/**
 * ModelViewer.cpp - MDL model viewer module.
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

#include "ModelViewer.hpp"
#include "../ui_helpers.hpp"

#include <imgui.h>


/** Minimum zoom level. */
static float const MIN_ZOOM = 0.5f;


ModelViewer::ModelViewer(Config &cfg)
:   Module{cfg, "Model Viewer", false, false}
,   _shader{{
        GLUtil::shader_from_file(
            "shaders/model.vert", GL_VERTEX_SHADER),
        GLUtil::shader_from_file(
            "shaders/model.frag", GL_FRAGMENT_SHADER)},
        "ModelShader"}
,   _model{
        "<none>",
        {   // bodyparts
            {   "QuadBodyPart",
                {   // models
                    {   "QuadModel",
                        {   // meshes
                            {   {   // tricmds
                                    {   false,
                                        {   // vertices
                                            {3, 0, 1,1},
                                            {1, 0, 0,1},
                                            {2, 0, 1,0},
                                            {0, 0, 0,0}}}},
                                0}
                        },
                        {   // vertices
                            { 1.0f, 1.0f, 0.0f}, // tl
                            { 1.0f,-1.0f, 0.0f}, // bl
                            {-1.0f, 1.0f, 0.0f}, // tr
                            {-1.0f,-1.0f, 0.0f}  // br
                        }}}}},
        {   // textures
            {   "<none>",
                2, 2,
                {0,1,2,3},
                {   0xff,0x00,0x00,
                    0x00,0xff,0x00,
                    0x00,0x00,0xff,
                    0xff,0xff,0xff}}},
        {   // skinref
            0
        }
    }
,   _glmodel{}
,   _textures{}
,   _selected{""}
,   _camera{}
,   _wireframe{false}
,   _transform{}
{
    _loadModel();
}

void ModelViewer::input(SDL_Event const *event)
{
    if (!gl_visible)
        return;
    switch (event->type)
    {
    case SDL_MOUSEMOTION:{
        // Hold middle mouse to orbit the camera.
        if (event->motion.state & SDL_BUTTON_MMASK)
            _camera.rotate({event->motion.xrel, event->motion.yrel});
        break;}
    case SDL_MOUSEWHEEL:{
        auto modstate = SDL_GetModState();
        // Scroll with ALT pressed to change FOV.
        if (modstate & KMOD_ALT)
            _camera.setFOV(_camera.fov - _cfg.mouse_sensitivity * event->wheel.y);
        // Scroll with nothing pressed to zoom.
        else
            _camera.setZoom(_camera.zoom - _cfg.mouse_sensitivity * event->wheel.y);
        break;}
    case SDL_KEYDOWN:{
        // Toggle wireframe with Z key.
        if (event->key.keysym.sym == SDLK_z)
        {
            glPolygonMode(GL_FRONT_AND_BACK, _wireframe? GL_LINE : GL_FILL);
            _wireframe = !_wireframe;
        }
        break;}
    }
}

void ModelViewer::drawUI()
{
    if (!ui_visible)
        return;

    if (ImGui::Begin(title.c_str(), &ui_visible))
    {
        ImGui::TextUnformatted(("Model: " + _model.name).c_str());
        if (ImGui::CollapsingHeader("Camera"))
            _camera.imgui();
        if (ImGui::CollapsingHeader("Model Transform"))
            ImGui::Transform(&_transform);
        ImGui::Separator();
        if (ImGui::BeginChild("ModelTree"))
        {
            if (ImGui::TreeNode("valve/models"))
            {
                if (ImGui::DirectoryTree(
                    _cfg.game_dir.string() + "/valve/models",
                    &_selected,
                    [](std::filesystem::path const &p){
                        return p.extension() == ".mdl";
                    }))
                {
                    _loadSelectedModel();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("valve_hd/models"))
            {
                if (ImGui::DirectoryTree(
                    _cfg.game_dir.string() + "/valve_hd/models",
                    &_selected,
                    [](std::filesystem::path const &p){
                        return p.extension() == ".mdl";
                    }))
                {
                    _loadSelectedModel();
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void ModelViewer::drawGL(float deltaT)
{
    // Setup projection matrix.
    auto const projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        *_cfg.window_width / (float)*_cfg.window_height,
        0.1f, 1000.0f);

    auto const modelMatrix = _transform.getMatrix();

    // Draw model.
    _shader.use();
    _glmodel.vao->bind();
    _glmodel.ebo->bind();
    glActiveTexture(GL_TEXTURE0);
    auto const &tex = _textures[0];
    tex.bind();
    _shader.setUniformS("model", modelMatrix);
    _shader.setUniformS("view", _camera.getViewMatrix());
    _shader.setUniformS("projection", projectionMatrix);
    _shader.setUniformS("tex", 0);
    for (size_t i = 0; i < _glmodel.count.size(); ++i)
    {
        _textures.at(_glmodel.texture.at(i)).bind();
        glDrawElements(
            GL_TRIANGLES, _glmodel.count.at(i), GL_UNSIGNED_INT,
            _glmodel.indices.at(i));
    }
}


void ModelViewer::_loadSelectedModel()
{
    _model = MDL::load_mdl(_selected.string());
    _loadModel();
}

void ModelViewer::_loadModel()
{
    _textures.clear();
    for (auto const &t : _model.textures)
        _textures.push_back(texture2GLTexture(t));
    _glmodel = model2vao(_model);
}
