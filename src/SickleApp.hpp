/**
 * SickleApp.hpp - Main Sickle app class.
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

#ifndef _SICKLEAPP_HPP
#define _SICKLEAPP_HPP

#include "common.hpp"
#include "modules/MapViewer.hpp"
#include "utils/ui_helpers.hpp"
#include "version.hpp"

#include <imgui.h>
#include <SDL.h>

#include <set>


/** Main app class. */
class SickleApp
{
private:
    MapViewer _mapEditor;
    Config &_cfg;
    bool _aboutWindowShown;

public:
    bool running;


    SickleApp(Config &cfg)
    :   _mapEditor{cfg}
    ,   _cfg{cfg}
    ,   _aboutWindowShown{false}
    ,   running{true}
    {
        _mapEditor.gl_visible = true;
        _mapEditor.ui_visible = true;
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
        _mapEditor.input(event);
    }

    /** Draw the app's UI. */
    void drawUI()
    {
        bool fpopen = false;

        // Main menu bar.
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
                ;// TODO: Create a new map
            if (ImGui::MenuItem("Open"))
                ;// TODO: Open a .map file
            if (ImGui::MenuItem("Exit"))
                running = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Set Game Directory"))
                fpopen = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem(_mapEditor.title.c_str()))
                _mapEditor.ui_visible = true;
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
        ImGui::FilePicker("Select Game Directory", &_cfg.game_dir);
        if (fpopen)
            ImGui::OpenPopup("Select Game Directory");

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
        _mapEditor.drawUI();
    }


    /** Draw non-UI app visuals. */
    void drawGL(float deltaT)
    {
        _mapEditor.drawGL(deltaT);
    }
};

#endif
