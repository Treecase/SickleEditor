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

#include "TextureViewer.hpp"
#include "mdl2gl.hpp"
#include "ui_helpers.hpp"

#include <imgui.h>

#include <iostream>


TextureViewer::TextureViewer(Config &cfg)
:   Module{cfg, "Texture Viewer", false}
,   _shader{
        {   GLUtil::shader_from_file(
                "shaders/vertex.vert", GL_VERTEX_SHADER),
            GLUtil::shader_from_file(
                "shaders/fragment.frag", GL_FRAGMENT_SHADER)},
        "ScreenQuadShader"}
,   _vao{"ScreenQuadVAO"}
,   _models{{"", {"<none>", {}, {{"<none>", 1, 1, {0}, {0}}}}}}
,   _textures{{"", {texture2GLTexture(_models[""].textures[0])}}}
,   _selected_model{""}
,   _current_texture{0}
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

void TextureViewer::input(SDL_Event const *event)
{
}

void TextureViewer::drawUI()
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
                std::filesystem::path p{_selected_model};
                if (ImGui::DirectoryTree(
                        _cfg.game_dir.string() + "/valve/models",
                        &p,
                        [](std::filesystem::path const &p){
                            return (
                                *(p.stem().string().end() - 1) != 't'
                                && *(p.stem().string().end() - 2) != '0'
                                && p.extension() == ".mdl"
                            );
                        }))
                {
                    _selected_model = std::filesystem::canonical(p).string();
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


void TextureViewer::_loadSelectedModel_MDL()
{
    try
    {
        _models.at(_selected_model);
        return;
    }
    catch (std::out_of_range const &)
    {
    }
    _models[_selected_model] = MDL::load_mdl(_selected_model);
}

void TextureViewer::_loadSelectedModel_GL()
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

void TextureViewer::_loadSelectedModel()
{
    _loadSelectedModel_MDL();
    _loadSelectedModel_GL();
}


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
