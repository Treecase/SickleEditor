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

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL.h>

#include <filesystem>
#include <vector>


/** Displays Models contained in a .MDL file. */
class ModelViewer
{
private:
    /** Vertex info. */
    struct VertDef {
        GLfloat x, y, z;    // Position
        GLfloat s, t;       // UV
        GLfloat r, g, b;    // Vertex color
    };

    // Shader.
    GLUtil::Program _shader;
    // Model VAO.
    GLUtil::VertexArray _vao;

    // Loaded MDL.
    MDL::Model _model;
    // List of loaded MDL's associated GL textures.
    std::vector<GLUtil::Texture> _textures;
    // Loaded MDL's vertices.
    std::vector<VertDef> _modelVertices{
        //   positions  texcoords    vertexcolors
        //   x      y     z      s     t      r     g     b
        // Top left tri
        { 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f}, // tl
        {-1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f}, // tr
        { 1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f, 1.0f}, // bl
        // Bottom right tri
        {-1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f}, // tr
        {-1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f}, // br
        { 1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f, 1.0f}, // bl
    };
    // Path to loaded MDL.
    std::filesystem::path _selected;

    // Reference to app config.
    Config &_cfg;

    // Orbiting camera.
    struct Camera {
        glm::vec2 angle;    // x/y angle
        GLfloat zoom;       // Distance from origin
        GLfloat fov;        // FOV
    } _camera;

    // Wireframe display toggle.
    bool _wireframe = false;

    // Model matrix.
    glm::mat4 _modelM;
    // Projection matrix.
    glm::mat4 _projectionM;

    /** Called when _selected is updated. */
    void _loadSelectedModel();

public:
    // App title.
    std::string title;
    // Is the UI visible?
    bool ui_visible;


    ModelViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event);

    /** Draw the app's UI. */
    void drawUI();

    /** Draw non-UI app visuals. */
    void drawGL();
};

#endif
