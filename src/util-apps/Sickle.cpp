/**
 * Sickle.cpp - Sickle Editor entry point.
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

#include "../App.hpp"
#include "../common.hpp"
#include "version.hpp"

#include "../modules/BSPViewer.hpp"
#include "../modules/MapViewer.hpp"
#include "../modules/ModelViewer.hpp"
#include "../modules/SoundPlayer.hpp"
#include "../modules/TextureViewer.hpp"
#include "../modules/WADTextureViewer.hpp"

#include <iostream>
#include <string>


char const *const APP_title = "Sickle Editor";
std::string const APP_canon_name = "sickle";
std::string const APP_version = "0.1.0";

typedef Config APP_Config;
typedef App<
        BSPViewer,
        MapViewer,
        ModelViewer,
        SoundPlayer,
        TextureViewer,
        WADTextureViewer
    > APP_Type;


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
        "Usage: " << name << " GAMEDEF.fgd MAPSDIR [GAMEDIR]\n"
        "Print .fgd files.\n"
        "\n";
}

/** Print short usage info for when APP_handle_args fails. */
void print_usage_short(char const *name)
{
    std::cout <<
        "Usage: " << name << " GAMEDEF.fgd MAPSDIR [GAMEDIR]\n"
        "Try '" << name << " --help' for more information.\n";
}

APP_Config APP_handle_args(int argc, char *argv[])
{
    APP_Config cfg{};
    // TODO: these shouldn't error out
    if (argc < 2)
    {
        print_usage_short(argv[0]);
        exit(EXIT_FAILURE);
    }
    else
        cfg.game_def = argv[1];
    if (argc < 3)
    {
        print_usage_short(argv[0]);
        exit(EXIT_FAILURE);
    }
    else
        cfg.maps_dir = argv[2];

    if (argc < 4)
        cfg.game_dir = "/";
    else
        cfg.game_dir = argv[3];
    return cfg;
}


#include "GUI-main.cpp"
