/**
 * Base3DViewer.hpp - Generic 1st-person 3D viewer.
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

#ifndef _BASE3DVIEWER_HPP
#define _BASE3DVIEWER_HPP

#include "utils/FreeCam.hpp"
#include "utils/ui_helpers.hpp"
#include "Module.hpp"

#include <glm/glm.hpp>
#include <glutils/glutils.hpp>


/** Displays BSP files. */
class Base3DViewer : public Module
{
protected:
    // Shader.
    GLUtil::Program _shader;
    // Camera.
    FreeCam _camera;
    // Wireframe display toggle.
    bool _wireframe;
    /** Speed multiplier when shift key is held down. */
    float _shift_multiplier;

    void setWireframe(bool value)
    {
        glPolygonMode(GL_FRONT_AND_BACK, value? GL_LINE : GL_FILL);
        _wireframe = value;
    }

public:
    Base3DViewer(
        Config &cfg, std::string const &title, bool visible, bool glrender,
        GLUtil::Program &&shader, FreeCam &&camera, bool wireframe,
        float shift_multiplier
    )
    :   Module(cfg, title, visible, glrender)
    ,   _shader{shader}
    ,   _camera{camera}
    ,   _wireframe{wireframe}
    ,   _shift_multiplier{shift_multiplier}
    {}


    /** Handle user input. */
    void input(SDL_Event const *event) override
    {
        if (!gl_visible)
            return;
        switch (event->type)
        {
        case SDL_MOUSEMOTION:{
            // Hold middle mouse to orbit the camera.
            if (event->motion.state & SDL_BUTTON_MMASK)
                _camera.rotate({event->motion.xrel, event->motion.yrel});
            break;}
        case SDL_MOUSEWHEEL:{
            auto modstate = SDL_GetModState();
            // Scroll with ALT pressed to change FOV.
            if (modstate & KMOD_ALT)
                _camera.setFOV(_camera.fov - _cfg.mouse_sensitivity * event->wheel.y);
            break;}
        case SDL_KEYDOWN:{
            // Toggle wireframe with Z key.
            if (event->key.keysym.sym == SDLK_z)
                setWireframe(!_wireframe);
            break;}
        }
    }

    /** Draw the app's UI. */
    void drawUI() override
    {
        if (ImGui::CollapsingHeader("Camera"))
            ImGui::FreeCam(&_camera);
        ImGui::Checkbox("Wireframe", &_wireframe);
        setWireframe(_wireframe);
    }

    /** Update. */
    void drawGL(float deltaT) override
    {
        // Get keyboard state.
        auto const keyState = SDL_GetKeyboardState(nullptr);
        auto const modState = SDL_GetModState();
        auto const shift = (modState & KMOD_SHIFT) != 0;

        glm::vec3 const movement_delta{
            keyState[SDL_SCANCODE_A] - keyState[SDL_SCANCODE_D],
            keyState[SDL_SCANCODE_Q] - keyState[SDL_SCANCODE_E],
            keyState[SDL_SCANCODE_W] - keyState[SDL_SCANCODE_S]};
        // Move the camera.
        if (glm::length(movement_delta) > 0.0f)
        {
            _camera.translate(
                deltaT
                * (shift? _shift_multiplier : 1.0f)
                * _camera.speed
                * glm::normalize(movement_delta));
        }
    }
};

#endif
