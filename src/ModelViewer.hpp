/**
 * ModelViewer.hpp - MDL model viewer module.
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

#ifndef _MODELVIEWER_HPP
#define _MODELVIEWER_HPP

#include "common.hpp"
#include "glUtils/glUtil.hpp"
#include "load_model.hpp"
#include "mdl2gl.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL.h>

#include <filesystem>
#include <vector>


/** Displays Models contained in a .MDL file. */
class ModelViewer : public Module
{
private:
    // Shader.
    GLUtil::Program _shader;

    // Loaded MDL.
    MDL::Model _model;
    // Model GL data.
    ModelDef _glmodel;
    // List of loaded MDL's associated GL textures.
    std::vector<GLUtil::Texture> _textures;
    // Path to loaded MDL.
    std::filesystem::path _selected;

    // Orbiting camera.
    struct Camera {
        glm::vec2 angle;    // x/y angle
        GLfloat zoom;       // Distance from origin
        GLfloat fov;        // FOV
    } _camera;

    // Wireframe display toggle.
    bool _wireframe;

    // Model matrix.
    glm::mat4 _modelM;
    // Model scaling.
    GLfloat _scale;

    /** Called when _selected is updated. */
    void _loadSelectedModel();
    void _loadModel();

public:
    ModelViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the app's UI. */
    void drawUI() override;

    /** Draw non-UI app visuals. */
    void drawGL() override;
};

#endif
