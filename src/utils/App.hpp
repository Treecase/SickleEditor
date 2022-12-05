/**
 * App.hpp - Main app class.
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

#ifndef _APP_HPP
#define _APP_HPP

#include "common.hpp"
#include "modules/Module.hpp"
#include "utils/ui_helpers.hpp"
#include "version.hpp"

#include <imgui.h>
#include <SDL.h>

#include <set>


/** Main app class. */
template<class... Ts>
class App
{
private:
    struct ModuleComparer
    {   bool operator()(
            std::shared_ptr<Module> const &a,
            std::shared_ptr<Module> const &b) const
        {return a->title < b->title;}
    };
    std::set<std::shared_ptr<Module>, ModuleComparer> _modules;
    Module *_activeGLDisplay;
    Config &_cfg;
    bool _aboutWindowShown;

public:
    bool running;


    App(Config &cfg)
    :   _modules{std::shared_ptr<Module>{new Ts{cfg}}...}
    ,   _activeGLDisplay{nullptr}
    ,   _cfg{cfg}
    ,   _aboutWindowShown{false}
    ,   running{true}
    {
        if (_modules.size() == 1)
        {
            _activeGLDisplay = (*_modules.cbegin()).get();
            (*_modules.cbegin())->gl_visible = true;
            (*_modules.cbegin())->ui_visible = true;
        }
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
        for (auto &module : _modules)
            module->input(event);
    }

    /** Draw the app's UI. */
    void drawUI()
    {
        bool fpopen = false;

        // Main menu bar.
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Game Directory"))
                fpopen = true;
            if (ImGui::MenuItem("Exit"))
                running = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("<none>"))
                _activeGLDisplay = nullptr;
            for (auto &module : _modules)
                if (ImGui::MenuItem(module->title.c_str()))
                {
                    if (_activeGLDisplay)
                        _activeGLDisplay->gl_visible = false;
                    _activeGLDisplay = module.get();
                    _activeGLDisplay->gl_visible = true;
                    module->ui_visible = true;
                }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
                _aboutWindowShown = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        ImGui::ShowMetricsWindow();

        // File picker (activated by File>Game Directory).
        ImGui::FilePicker("File Picker", &_cfg.game_dir);
        if (fpopen)
            ImGui::OpenPopup("File Picker");

        // About Window (activated by Help>About).
        if (_aboutWindowShown)
        {
            if (ImGui::Begin("About##Help/About", &_aboutWindowShown))
            {
                ImGui::TextWrapped(SE_CANON_NAME " " SE_VERSION);
                ImGui::NewLine();
                ImGui::TextWrapped("Copyright (C) 2022 Trevor Last");
                if (ImGui::Button("Close"))
                    _aboutWindowShown = false;
            }
            ImGui::End();
        }

        // Draw modules.
        for (auto &module : _modules)
            module->drawUI();
    }


    /** Draw non-UI app visuals. */
    void drawGL(float deltaT)
    {
        if (_activeGLDisplay != nullptr)
            _activeGLDisplay->drawGL(deltaT);
    }
};

#endif
