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
#include "mdl/load_model.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <glutils/glutils.hpp>
#include <SDL.h>

#include <unordered_map>
#include <vector>


/** Displays Textures contained in a .MDL file. */
class TextureViewer: public Module
{
private:
    // Screenquad vertex data.
    static std::vector<GLfloat> const _sqv;

    // Screenquad shader.
    GLUtil::Program _shader;
    // Screenquad VAO.
    GLUtil::VertexArray _vao;

    // Loaded MDLs.
    std::unordered_map<std::string, MDL::Model> _models;
    // Map of MDLs loaded to a list of their associated GL textures.
    std::unordered_map<std::string, std::vector<GLUtil::Texture>> _textures;
    // Path to currently displayed model.
    std::string _selected_model;
    // Index of currently displayed texture.
    int _current_texture;

    void _loadSelectedModel_MDL();
    void _loadSelectedModel_GL();
    void _loadSelectedModel();

public:
    TextureViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the app's UI. */
    void drawUI() override;

    /** Does nothing. */
    void drawGL(float deltaT) override {};
};

#endif
