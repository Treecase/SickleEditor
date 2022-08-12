/**
 * TextureViewer.hpp - MDL texture viewer module.
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

#ifndef _TEXTUREVIEWER_HPP
#define _TEXTUREVIEWER_HPP

#include "common.hpp"
#include "glUtils/glUtil.hpp"
#include "load_model.hpp"
#include "mdl2gl.hpp"
#include "ui_helpers.hpp"
#include "version.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <SDL.h>

#include <iostream>
#include <unordered_map>
#include <vector>


/** Displays Textures contained in a .MDL file. */
class TextureViewer
{
private:
    // Screenquad vertex data.
    static std::vector<GLfloat> const _sqv;
    // Screenquad shader.
    GLUtil::Program _shader;
    // Screenquad VAO.
    GLUtil::VertexArray _vao;
    // Loaded MDLs.
    std::unordered_map<std::filesystem::path, MDL::Model> _models;
    // Map of MDLs loaded to a list of their associated GL textures.
    std::unordered_map<std::filesystem::path, std::vector<GLUtil::Texture>>
        _textures;
    // Path to currently displayed model.
    std::filesystem::path _selected_model;
    // Index of currently displayed texture.
    int _current_texture;
    Config &_cfg;

    void _loadSelectedModel_MDL()
    {
        try
        {
            _models.at(_selected_model);
            return;
        }
        catch (std::out_of_range const &)
        {
        }
        _models[_selected_model] = MDL::load_mdl(_selected_model.string());
    }
    void _loadSelectedModel_GL()
    {
        try
        {
            _textures.at(_selected_model).at(_current_texture);
            return;
        }
        catch (std::out_of_range const &)
        {
        }
        _textures[_selected_model] = std::vector<GLUtil::Texture>{};
        for (auto const &t : _models[_selected_model].textures)
        {
            _textures[_selected_model].push_back(texture2GLTexture(t));
        }
    }
    void _loadSelectedModel()
    {
        _loadSelectedModel_MDL();
        _loadSelectedModel_GL();
    }

public:
    // App title.
    std::string title;
    bool ui_visible;


    TextureViewer(Config &cfg)
    :   _shader{
            {   GLUtil::shader_from_file(
                    "shaders/vertex.vert", GL_VERTEX_SHADER),
                GLUtil::shader_from_file(
                    "shaders/fragment.frag", GL_FRAGMENT_SHADER)},
            "ScreenQuadShader"}
    ,   _vao{"ScreenQuadVAO"}
    ,   _models{{"", {"<none>", {{"<none>", 1, 1, {0}, {0}}}}}}
    ,   _textures{{"", {texture2GLTexture(_models[""].textures[0])}}}
    ,   _selected_model{""}
    ,   _current_texture{0}
    ,   _cfg{cfg}
    ,   title{"Texture Viewer"}
    ,   ui_visible{false}
    {
        _vao.bind();
        // Screenquad vbo.
        GLUtil::Buffer vbo{GL_ARRAY_BUFFER, "ScreenQuadVBO"};
        vbo.bind();
        vbo.buffer(GL_STATIC_DRAW, _sqv);
        // Positions array.
        _vao.enableVertexAttribArray(0, 3, GL_FLOAT, 5 * sizeof(GLfloat));
        // UV array.
        _vao.enableVertexAttribArray(
            1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
        vbo.unbind();
        _vao.unbind();
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
    }

    /** Draw the app's UI. */
    void drawUI()
    {
        if (!ui_visible)
            return;

        auto const &model_name = _models[_selected_model].name;
        auto const &mdl_tex = _models[_selected_model].textures[_current_texture];
        auto const &tex = _textures[_selected_model][_current_texture];
        int tex_w, tex_h;
        tex.bind();
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);
        tex.unbind();

        if (ImGui::Begin(title.c_str(), &ui_visible))
        {
            ImGui::TextUnformatted(model_name.c_str());
            ImGui::SliderInt(
                "Texture", &_current_texture, 0,
                _textures[_selected_model].size() - 1, "%d",
                ImGuiSliderFlags_AlwaysClamp);
            ImGui::TextUnformatted(mdl_tex.name.c_str());
            ImGui::TextUnformatted(
                (   "Originally " + std::to_string(mdl_tex.w) + "x"
                    + std::to_string(mdl_tex.h)).c_str());
            ImGui::TextUnformatted(
                (   "Sampled to " + std::to_string(tex_w) + "x"
                    + std::to_string(tex_h)).c_str());
            ImGui::Image((void*)(intptr_t)tex.id(), ImVec2(tex_w, tex_h));
            ImGui::Separator();
            if (ImGui::BeginChild("ModelTree"))
            {
                if (ImGui::TreeNode("valve/models"))
                {
                    if (ImGui::DirectoryTree(
                            _cfg.game_dir.string() + "/valve/models",
                            &_selected_model,
                            [](std::filesystem::path const &p){
                                return (
                                    *(p.stem().string().end() - 1) != 't'
                                    && *(p.stem().string().end() - 2) != '0'
                                    && p.extension() == ".mdl"
                                );
                            }))
                    {
                        _current_texture = 0;
                        _loadSelectedModel();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    /** Draw non-UI app visuals. */
    void drawGL()
    {
        if (_selected_model.empty())
            return;

        // Draw screen.
        _shader.use();
        _vao.bind();
        glActiveTexture(GL_TEXTURE0);
        auto const &tex = _textures[_selected_model][_current_texture];
        tex.bind();
        _shader.setUniformS("tex", 0);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(_sqv.size() / 5));
    }
};


std::vector<GLfloat> const TextureViewer::_sqv{
    // Positions        Texcoords
    // Top right tri
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // tl
    1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // tr
    1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // br
    // Bottom left tri
    1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // br
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // bl
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // tl
};

#endif
