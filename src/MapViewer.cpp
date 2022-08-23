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
#include "ui_helpers.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>


#define CLAMP(x, min, max) ((x) < (min)? (min) : (x) > (max)? (max) : (x))

#define MOUSE_SENSITIVITY 0.5f
#define MIN_ZOOM 0.5f
#define MIN_FOV 30.0f
#define MAX_FOV 90.0f


MapViewer::MapViewer(Config &cfg)
:   Module{cfg, "Map Viewer", false}
,   _shader{{
        GLUtil::shader_from_file(
            "shaders/map.vert", GL_VERTEX_SHADER),
        GLUtil::shader_from_file(
            "shaders/map.frag", GL_FRAGMENT_SHADER)},
        "MapShader"}
,   _map{}
,   _glbsp{}
,   _selected{""}
,   _camera{{0.0f, 0.0f}, 2.0f, 70.0f}
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
            _camera.angle.y = fmod(
                _camera.angle.y + glm::radians((GLfloat)event->motion.yrel),
                glm::two_pi<GLfloat>());
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
        // Scroll with nothing pressed to zoom.
        else
        {
            _camera.zoom -= MOUSE_SENSITIVITY * event->wheel.y;
            if (_camera.zoom < MIN_ZOOM)
                _camera.zoom = MIN_ZOOM;
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
        ImGui::TextUnformatted(("Map: " + _selected.string()).c_str());
        ImGui::DragFloat("Zoom", &_camera.zoom, 1.0f, MIN_ZOOM, FLT_MAX);
        ImGui::SliderFloat("FOV", &_camera.fov, MIN_FOV, MAX_FOV);
        ImGui::Value("Pitch", glm::degrees(_camera.angle.y));
        ImGui::Value("Yaw", glm::degrees(_camera.angle.x));
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

void MapViewer::drawGL()
{
    // Setup view matrix.
    glm::vec3 pos{0.0f, 0.0f, -_camera.zoom};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    auto viewMatrix = glm::rotate(
        glm::rotate(
            glm::lookAt(pos, glm::vec3{0.0f}, up),
            _camera.angle.y,
            glm::cross(up, pos)),
        _camera.angle.x,
        up);

    // Setup projection matrix.
    auto projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        *_cfg.window_width / (float)*_cfg.window_height,
        0.1f, 1000.0f);

    // TODO: rotation's a bit weird?
    auto modelMatrix =
        glm::rotate(
            glm::rotate(
                glm::rotate(
                    glm::scale(
                        glm::translate(
                            glm::identity<glm::mat4>(),
                            glm::vec3{-_translation[0], _translation[1], _translation[2]}
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

    // Draw model.
    _shader.use();
    _glbsp.vao->bind();
    _glbsp.ebo->bind();
    // glActiveTexture(GL_TEXTURE0);
    // auto const &tex = _textures[0];
    // tex.bind();
    _shader.setUniformS("model", modelMatrix);
    _shader.setUniformS("view", viewMatrix);
    _shader.setUniformS("projection", projectionMatrix);
    // _shader.setUniformS("tex", 0);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(-1);
    glDrawElements(
        GL_TRIANGLE_FAN,
        _glbsp.indices.size(),
        GL_UNSIGNED_INT,
        (void *)0
    );
}


void MapViewer::_loadSelectedMap()
{
    if (!_selected.empty())
        _map = BSP::load_bsp(_selected.string());
    _loadMap();
}

void MapViewer::_loadMap()
{
    if (_selected.empty())
    {
        _glbsp = GLBSP{
            {},
            {},
            std::shared_ptr<GLUtil::VertexArray>{
                new GLUtil::VertexArray{"mapVAO"}},
            std::shared_ptr<GLUtil::Buffer>{
                new GLUtil::Buffer{GL_ARRAY_BUFFER, "mapVBO"}},
            std::shared_ptr<GLUtil::Buffer>{
                new GLUtil::Buffer{GL_ELEMENT_ARRAY_BUFFER, "mapEBO"}}
        };
        _glbsp.vao->enableVertexAttribArray(
            0, 3, GL_FLOAT, sizeof(BSPVertexDef), offsetof(BSPVertexDef, x));
        _glbsp.vao->enableVertexAttribArray(
            1, 2, GL_FLOAT, sizeof(BSPVertexDef), offsetof(BSPVertexDef, s));
        _glbsp.ebo->unbind();
        _glbsp.vbo->unbind();
        _glbsp.vao->unbind();
        return;
    }
    else
    {
        _glbsp = bsp2gl(_map);
    }
}
