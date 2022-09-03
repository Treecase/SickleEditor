/**
 * WADTextureViewer.hpp - WAD texture viewer module.
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

#ifndef _WADTEXTUREVIEWER_HPP
#define _WADTEXTUREVIEWER_HPP

#include "common.hpp"
#include "wad/load_wad.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <glutils/glutils.hpp>
#include <SDL.h>

#include <unordered_map>
#include <vector>


/** Displays Textures contained in a .WAD file. */
class WADTextureViewer: public Module
{
private:
    // GL Textures.
    std::vector<GLUtil::Texture> _textures;
    // Loaded WAD.
    WAD::WAD _wad;
    // Path to current WAD.
    std::filesystem::path _selected;
    // Index of currently displayed texture.
    int _current_texture;

    void _loadSelected();
    void _loadSelected_GL();

public:
    WADTextureViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the app's UI. */
    void drawUI() override;

    /** Does nothing. */
    void drawGL(float deltaT) override {};
};

#endif
