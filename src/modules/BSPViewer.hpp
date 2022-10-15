/**
 * BSPViewer.hpp - Map viewer module.
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

#ifndef _BSPVIEWER_HPP
#define _BSPVIEWER_HPP

#include "../common.hpp"
#include "../bsp/bsp2gl.hpp"
#include "../bsp/load_bsp.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glutils/glutils.hpp>
#include <SDL.h>

#include <filesystem>


/** Displays BSP files. */
class BSPViewer : public Module
{
private:
    // Shader.
    GLUtil::Program _shader;

    // Loaded map.
    BSP::BSP _map;
    // GL map.
    BSP::GLBSP _glbsp;
    // Path to loaded map.
    std::filesystem::path _selected;

    // First-person camera.
    struct Camera {
        glm::vec3 pos;      // Position
        glm::vec2 angle;    // x/y angle
        GLfloat fov;        // FOV
        GLfloat speed;      // Movement speed
    } _camera;

    // Wireframe display toggle.
    bool _wireframe;

    // Map translation.
    GLfloat _translation[3];
    // Map rotation.
    GLfloat _rotation[3];
    // Map scaling.
    GLfloat _scale;

    /** Called when _selected is updated. */
    void _loadSelectedMap();
    void _loadMap();

public:
    BSPViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the module's UI. */
    void drawUI() override;

    /** Draw non-UI module visuals. */
    void drawGL(float deltaT) override;
};

#endif
