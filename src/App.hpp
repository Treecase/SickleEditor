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
#include "ModelViewer.hpp"
#include "SoundPlayer.hpp"
#include "TextureViewer.hpp"
#include "version.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <SDL.h>


/** Main app class. */
class App
{
private:
    ModelViewer _modelViewer;
    SoundPlayer _soundPlayer;
    TextureViewer _textureViewer;
    Config &_cfg;

public:
    bool running;


    App(Config &cfg)
    :   _modelViewer{cfg}
    ,   _soundPlayer{cfg}
    ,   _textureViewer{cfg}
    ,   _cfg{cfg}
    ,   running{true}
    {
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
        _modelViewer.input(event);
        _soundPlayer.input(event);
        _textureViewer.input(event);
    }

    /** Draw the app's UI. */
    void drawUI()
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
            if (ImGui::MenuItem("Model Viewer"))
                _modelViewer.ui_visible = true;
            if (ImGui::MenuItem("Sound Player"))
                _soundPlayer.ui_visible = true;
            if (ImGui::MenuItem("Texture Viewer"))
                _textureViewer.ui_visible = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        ImGui::ShowMetricsWindow();

        _modelViewer.drawUI();
        _soundPlayer.drawUI();
        _textureViewer.drawUI();
    }

    /** Draw non-UI app visuals. */
    void drawGL()
    {
        _modelViewer.drawGL();
        // _textureViewer.drawGL();
    }
};

#endif
