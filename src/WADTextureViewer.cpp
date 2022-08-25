/**
 * WADTextureViewer.cpp - WAD texture viewer module.
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

#include "WADTextureViewer.hpp"
#include "ui_helpers.hpp"

#include <imgui.h>

#include <iostream>


WADTextureViewer::WADTextureViewer(Config &cfg)
:   Module{cfg, "WAD Texture Viewer", false, false}
,   _textures{}
,   _wad{}
,   _selected{""}
,   _current_texture{0}
,   _paletteoffset{0}
{
    _loadSelected_GL();
}

void WADTextureViewer::input(SDL_Event const *event)
{
}

void WADTextureViewer::drawUI()
{
    if (!ui_visible)
        return;

    if (ImGui::Begin(title.c_str(), &ui_visible))
    {
        ImGui::TextUnformatted(("WAD: " + _selected.filename().string()).c_str());
        if (_textures.size() > 0)
        {
            ImGui::TextUnformatted(("Texture: " + _wad.directory[_current_texture].name).c_str());
            auto const &tex = _textures[_current_texture];
            int tex_w, tex_h;
            tex.bind();
            glGetTexLevelParameteriv(tex.type(), 0, GL_TEXTURE_WIDTH, &tex_w);
            glGetTexLevelParameteriv(tex.type(), 0, GL_TEXTURE_HEIGHT, &tex_h);
            tex.unbind();
            ImGui::SliderInt("Texture", &_current_texture, 0, _textures.size() - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Value("Width", tex_w);
            ImGui::Value("Height", tex_h);
            ImGui::Image((void*)(intptr_t)tex.id(), ImVec2(tex_w, tex_h));
        }
        if (ImGui::SliderInt("Palette Offset", &_paletteoffset, 0, 836))
            _loadSelected_GL();
        ImGui::Separator();
        if (ImGui::BeginChild("ModelTree"))
        {
            if (ImGui::TreeNode("valve"))
            {
                if (ImGui::DirectoryTree(
                        _cfg.game_dir.string() + "/valve",
                        &_selected,
                        [](std::filesystem::path const &p){
                            return p.extension() == ".wad";
                        }))
                {
                    _current_texture = 0;
                    _loadSelected();
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}


void WADTextureViewer::_loadSelected()
{
    _wad = WAD::load(_selected.string());
    _loadSelected_GL();
}

#include <fstream>

void WADTextureViewer::_loadSelected_GL()
{
    _textures.clear();
    for (auto const &lump : _wad.directory)
    {
        if (lump.type != 0x43)
            continue;

        auto const ptr = lump.data.data();
        char name[16];
        uint32_t w, h;
        memcpy(name, ptr, 16);
        memcpy(&w, ptr + 16, 4);
        memcpy(&h, ptr + 20, 4);
        uint32_t ptrs[4];
        memcpy(ptrs, ptr + 24, 4 * 4);

        auto depaletted = new uint8_t[w * h * 4];
        for (size_t i = 0, j = 0; i < w * h; ++i, j += 4)
        {
            auto pixel = (ptr + ptrs[0])[i];
            auto const palette = ptr + ptrs[3] + _paletteoffset;
            depaletted[j+0] = (palette+pixel)[2];
            depaletted[j+1] = (palette+pixel)[1];
            depaletted[j+2] = (palette+pixel)[0];
            depaletted[j+3] = 0xff;
        }

        GLUtil::Texture t{GL_TEXTURE_2D, name};
        t.bind();
        t.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(
            t.type(), 0, GL_RGBA, w, h, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, depaletted);
        t.unbind();
        delete[] depaletted;
        _textures.push_back(t);
    }
}
