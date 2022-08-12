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
#include "mdl2gl.hpp"
#include "ui_helpers.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>


#define CLAMP(x, min, max) ((x) < (min)? (min) : (x) > (max)? (max) : (x))

#define MOUSE_SENSITIVITY 0.5f
#define MIN_ZOOM 0.1f
#define MIN_FOV 30.0f
#define MAX_FOV 90.0f


ModelViewer::ModelViewer(Config &cfg)
:   _shader{{
        GLUtil::shader_from_file(
            "shaders/model.vert", GL_VERTEX_SHADER),
        GLUtil::shader_from_file(
            "shaders/model.frag", GL_FRAGMENT_SHADER)},
        "ModelShader"}
,   _vao{"ModelVAO"}
,   _model{"<none>", {{"<none>", 1, 1, {0}, {0xff, 0xff, 0xff}}}}
,   _textures{texture2GLTexture(_model.textures[0])}
,   _selected{""}
,   _cfg{cfg}
,   _camera{{0.0f, 0.0f}, 2.0f, 70.0f}
,   _modelM{glm::identity<glm::mat4>()}
,   _projectionM{glm::perspective(
        glm::radians(_camera.fov),
        *cfg.window_width / (float)*cfg.window_height,
        0.1f, 100.0f)}
,   title{"Model Viewer"}
,   ui_visible{false}
{
    _vao.bind();
    // Model vbo.
    GLUtil::Buffer vbo{GL_ARRAY_BUFFER, "ModelVBO"};
    vbo.bind();
    vbo.buffer(GL_STATIC_DRAW, _modelVertices);
    // Positions array.
    _vao.enableVertexAttribArray(0, 3, GL_FLOAT, 8 * sizeof(GLfloat));
    // UV array.
    _vao.enableVertexAttribArray(
        1, 2, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    // Vertex Color array.
    _vao.enableVertexAttribArray(
        2, 3, GL_FLOAT, 8 * sizeof(GLfloat), 5 * sizeof(GLfloat));
    vbo.unbind();
    _vao.unbind();
}

void ModelViewer::input(SDL_Event const *event)
{
    switch (event->type)
    {
    case SDL_MOUSEMOTION:{
        // Hold middle mouse to orbit the camera.
        if (event->motion.state & SDL_BUTTON_MMASK)
        {
            _camera.angle.x = fmod(
                _camera.angle.x + glm::radians<GLfloat>(event->motion.xrel),
                glm::two_pi<GLfloat>());
            _camera.angle.y = fmod(
                _camera.angle.y + glm::radians<GLfloat>(event->motion.yrel),
                glm::two_pi<GLfloat>());
        }
        break;}
    case SDL_MOUSEWHEEL:{
        auto modstate = SDL_GetModState();
        // Scroll with nothing pressed to zoom.
        if (modstate == 0)
        {
            _camera.zoom -= MOUSE_SENSITIVITY * event->wheel.y;
            if (_camera.zoom < MIN_ZOOM)
                _camera.zoom = MIN_ZOOM;
        }
        // Scroll with ALT pressed to change FOV.
        else if (modstate & KMOD_ALT)
        {
            _camera.fov -= MOUSE_SENSITIVITY * event->wheel.y;
            _camera.fov = CLAMP(_camera.fov, MIN_FOV, MAX_FOV);
            _projectionM = glm::perspective(
                glm::radians(_camera.fov),
                *_cfg.window_width / (float)*_cfg.window_height,
                0.1f, 100.0f);
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

void ModelViewer::drawUI()
{
    if (!ui_visible)
        return;

    if (ImGui::Begin(title.c_str(), &ui_visible))
    {
        ImGui::TextUnformatted(("Model: " + _selected.string()).c_str());
        ImGui::TextUnformatted(
            ("FOV: " + std::to_string(_camera.fov)).c_str());
        ImGui::TextUnformatted(
            (   "Pitch: "
                + std::to_string(glm::degrees(_camera.angle.y))).c_str());
        ImGui::TextUnformatted(
            (   "Yaw: "
                + std::to_string(glm::degrees(_camera.angle.x))).c_str());
        ImGui::Separator();
        if (ImGui::BeginChild("ModelTree"))
        {
            if (ImGui::TreeNode("valve/models"))
            {
                if (ImGui::DirectoryTree(
                    _cfg.game_dir.string() + "/valve/models",
                    &_selected,
                    [](std::filesystem::path const &p){
                        return (
                            *(p.stem().string().end() - 1) != 't'
                            && *(p.stem().string().end() - 2) != '0'
                            && p.extension() == ".mdl");
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

void ModelViewer::drawGL()
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

    // Draw model.
    _shader.use();
    _vao.bind();
    glActiveTexture(GL_TEXTURE0);
    auto const &tex = _textures[0];
    tex.bind();
    _shader.setUniformS("model", _modelM);
    _shader.setUniformS("view", viewMatrix);
    _shader.setUniformS("projection", _projectionM);
    _shader.setUniformS("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)_modelVertices.size());
}


void ModelViewer::_loadSelectedModel()
{
    _model = MDL::load_mdl(_selected.string());
    _textures.clear();
    for (auto const &t : _model.textures)
        _textures.push_back(texture2GLTexture(t));
}
