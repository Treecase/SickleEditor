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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>


#define CLAMP(x, min, max) ((x) < (min)? (min) : (x) > (max)? (max) : (x))

#define MOUSE_SENSITIVITY 0.5f
#define MIN_ZOOM 0.5f
#define MIN_FOV 30.0f
#define MAX_FOV 90.0f
#define SHIFT_MULTIPLIER 2.0f


MapViewer::MapViewer(Config &cfg)
:   Module{cfg, "Map Viewer", false, false}
,   _shader{{
        GLUtil::shader_from_file(
            "shaders/map.vert", GL_VERTEX_SHADER),
        GLUtil::shader_from_file(
            "shaders/map.frag", GL_FRAGMENT_SHADER)},
        "MapShader"}
,   _map{}
,   _glbsp{}
,   _selected{""}
,   _camera{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 70.0f, 5.0f}
,   _wireframe{false}
,   _translation{0.0f, 0.0f, 0.0f}
,   _rotation{-90.0f, 0.0f, 0.0f}
,   _scale{0.005f}
{
    _loadMap();
}

void MapViewer::input(SDL_Event const *event)
{
    switch (event->type)
    {
    case SDL_MOUSEMOTION:{
        // Hold middle mouse to orbit the camera.
        if (event->motion.state & SDL_BUTTON_MMASK)
        {
            _camera.angle.x = fmod(
                _camera.angle.x + glm::radians((GLfloat)event->motion.xrel),
                glm::two_pi<GLfloat>());
            _camera.angle.y = CLAMP(
                _camera.angle.y + glm::radians((GLfloat)event->motion.yrel),
                -glm::radians(89.0f),
                glm::radians(89.0f)
            );
        }
        break;}
    case SDL_MOUSEWHEEL:{
        auto modstate = SDL_GetModState();
        // Scroll with ALT pressed to change FOV.
        if (modstate & KMOD_ALT)
        {
            _camera.fov -= MOUSE_SENSITIVITY * event->wheel.y;
            _camera.fov = CLAMP(_camera.fov, MIN_FOV, MAX_FOV);
        }
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
        if (ImGui::CollapsingHeader("Camera"))
        {
            ImGui::SliderFloat("FOV", &_camera.fov, MIN_FOV, MAX_FOV);
            ImGui::DragFloat("Speed", &_camera.speed, 0.1f, 0.0f, FLT_MAX);
            ImGui::Text("Pos: %.3f %.3f %.3f", _camera.pos.x, _camera.pos.y, _camera.pos.z);
            ImGui::Value("Pitch", glm::degrees(_camera.angle.y));
            ImGui::Value("Yaw", glm::degrees(_camera.angle.x));
        }
        if (ImGui::CollapsingHeader("Map Transform"))
        {
            if (ImGui::Button("Reset"))
            {
                memset(_translation, 0, 3*sizeof(GLfloat));
                memset(_rotation, 0, 3*sizeof(GLfloat));
                _scale = 1.0f;
            }
            ImGui::DragFloat3("Translation", _translation, 0.01f);
            if (ImGui::DragFloat3("Rotation", _rotation, 0.5f))
            {
                _rotation[0] = fmod(_rotation[0], 360.0f);
                _rotation[1] = fmod(_rotation[1], 360.0f);
                _rotation[2] = fmod(_rotation[2], 360.0f);
            }
            ImGui::DragFloat("Scale", &_scale, 0.005f, 0.0f, FLT_MAX);
        }
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

void MapViewer::drawGL(float deltaT)
{
    if (_selected.empty())
        return;

    /* ===[ Camera Vectors ]=== */
    // Camera space cardinal directions.
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 lookDir{
        -glm::sin(_camera.angle.x), 0.0f, glm::cos(_camera.angle.x)};
    glm::vec3 side = glm::cross(up, lookDir);
    lookDir = glm::rotate(lookDir, _camera.angle.y, side);

    // Get keyboard state.
    auto keyState = SDL_GetKeyboardState(nullptr);
    auto modState = SDL_GetModState();
    auto shift = (modState & KMOD_SHIFT) != 0;

    glm::vec3 movement_factor{
        keyState[SDL_SCANCODE_D] - keyState[SDL_SCANCODE_A],
        keyState[SDL_SCANCODE_Q] - keyState[SDL_SCANCODE_E],
        keyState[SDL_SCANCODE_W] - keyState[SDL_SCANCODE_S]};
    glm::vec3 movement_delta = (
        - movement_factor.x * side
        + glm::vec3{0.0f, movement_factor.y, 0.0f}
        + movement_factor.z * lookDir);
    // Move the camera.
    if (glm::length(movement_delta) > 0.0f)
    {
        _camera.pos += (
            deltaT
            * (shift? SHIFT_MULTIPLIER : 1.0f)
            * _camera.speed
            * glm::normalize(movement_delta));
    }

    // Setup view matrix.
    auto viewMatrix = glm::lookAt(_camera.pos, _camera.pos + lookDir, up);

    // Setup projection matrix.
    auto projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        *_cfg.window_width / (float)*_cfg.window_height,
        0.1f, 1000.0f);

    // Draw models.
    _shader.use();
    _glbsp.vao->bind();
    _glbsp.ebo->bind();
    glActiveTexture(GL_TEXTURE0);
    _shader.setUniformS("view", viewMatrix);
    _shader.setUniformS("projection", projectionMatrix);
    _shader.setUniformS("tex", 0);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(-1);
    for (auto const &model : _glbsp.models)
    {
        // TODO: rotation's a bit weird?
        auto modelMatrix =
            glm::rotate(
                glm::rotate(
                    glm::rotate(
                        glm::scale(
                            glm::translate(
                                glm::identity<glm::mat4>(),
                                model.position + glm::vec3{-_translation[0], _translation[1], _translation[2]}
                            ),
                            glm::vec3{_scale}
                        ),
                        glm::radians(_rotation[1]),
                        glm::vec3{0.0f, 1.0f, 0.0f}
                    ),
                    glm::radians(_rotation[2]),
                    glm::vec3{0.0f, 0.0f, 1.0f}
                ),
                glm::radians(_rotation[0]),
                glm::vec3{1.0f, 0.0f, 0.0f}
        );
        _shader.setUniformS("model", modelMatrix);

        for (auto const &mesh : model.meshes)
        {
            mesh.tex.bind();
            glDrawElements(
                GL_TRIANGLE_FAN, mesh.count, GL_UNSIGNED_INT, mesh.indices);
        }
    }
}


void MapViewer::_loadSelectedMap()
{
    _map = BSP::load_bsp(_selected.string());
    _loadMap();
}

void MapViewer::_loadMap()
{
    _glbsp = BSP::bsp2gl(_map, _cfg.game_dir.string());
}
