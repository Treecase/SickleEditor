/**
 * ViewMDL.cpp - View .mdl files.
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

#include "utils/App.hpp"
#include "../common.hpp"
#include "version.hpp"

#include "../modules/ModelViewer.hpp"

#include <iostream>
#include <string>


static float const default_mouse_sensitivity = 0.5f;

char const *const APP_title = "View MDL";
std::string const APP_canon_name = "viewmdl";
std::string const APP_version = "0.1.0";

typedef Config APP_Config;
typedef App<ModelViewer> APP_Type;


void APP_init_SDL()
{
}

void APP_init_OpenGL()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
}

void APP_print_usage(char const *name)
{
    std::cout <<
        "Usage: " << name << " [GAMEDIR]\n"
        "View .mdl files.\n"
        "\n";
}

/** Print short usage info for when APP_handle_args fails. */
void print_usage_short(char const *name)
{
    std::cout <<
        "Usage: " << name << " [GAMEDIR]\n"
        "Try '" << name << " --help' for more information.\n";
}

APP_Config APP_handle_args(int argc, char *argv[])
{
    if (argc > 2)
    {
        print_usage_short(argv[0]);
        exit(EXIT_FAILURE);
    }
    APP_Config cfg{};
    cfg.game_dir = argv[1];
    cfg.mouse_sensitivity = default_mouse_sensitivity;
    return cfg;
}


#include "GUI-main.cpp"
