/**
 * App.cpp - Main app class.
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

#include "App.hpp"

#include <imgui.h>


App::App(Config &cfg)
:   _modules{
        std::shared_ptr<Module>{new ModelViewer{cfg}},
        std::shared_ptr<Module>{new SoundPlayer{cfg}},
        std::shared_ptr<Module>{new TextureViewer{cfg}},
    }
,   _cfg{cfg}
,   running{true}
{
}

void App::input(SDL_Event const *event)
{
    for (auto &module : _modules)
        module->input(event);
}

void App::drawUI()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Exit"))
            running = false;
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows"))
    {
        for (auto &module : _modules)
            if (ImGui::MenuItem(module->title.c_str()))
                module->ui_visible = true;
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    ImGui::ShowMetricsWindow();

    for (auto &module : _modules)
        module->drawUI();
}

void App::drawGL()
{
    for (auto &module : _modules)
        module->drawGL();
}
