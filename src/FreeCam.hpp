/**
 * FreeCam.hpp - First person camera.
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

#ifndef _FREECAM_HPP
#define _FREECAM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>


/** Freelook first-person camera. */
struct FreeCam
{
    glm::vec3 pos;      // Position
    glm::vec2 angle;    // x/y angle (radians)
    float fov;          // FOV (degrees)
    float speed;        // Movement speed
    float min_fov = 30.0f, // FOV min value
          max_fov = 90.0f; // FOV max value


    FreeCam()
    :   pos{0.0f, 0.0f, 0.0f}
    ,   angle{0.0f, 0.0f}
    ,   fov{70.0f}
    ,   speed{5.0f}
    {
    }

    /** Set camera angle. */
    void setAngle(glm::vec2 value)
    {
        static constexpr auto Y_LIMIT = glm::radians(89.0f);
        angle.x = fmod(value.x, glm::radians(360.0f));
        angle.y = glm::clamp(value.y, -Y_LIMIT, Y_LIMIT);
    }

    /** Set the camera FOV. */
    void setFOV(float fov_)
    {
        fov = glm::clamp(fov_, min_fov, max_fov);
    }


    /** Turn the camera by `delta` degrees. */
    void rotate(glm::vec2 delta)
    {
        setAngle(angle + glm::radians(delta));
    }

    /** Translate camera (local coords). */
    void translate(glm::vec3 delta)
    {
        pos += delta.x * getSideDirection();
        pos += delta.y * getUpDirection();
        pos += delta.z * getLookDirection();
    }

    /** Get camera up vector. */
    constexpr glm::vec3 getUpDirection() const
    {
        return {0.0f, 1.0f, 0.0f};
    }
    /** Get camera side vector. */
    glm::vec3 getSideDirection() const
    {
        glm::vec3 const look_dir{-glm::sin(angle.x), 0.0f, glm::cos(angle.x)};
        return glm::cross(getUpDirection(), look_dir);
    }
    /** Get camera forward vector. */
    glm::vec3 getLookDirection() const
    {
        glm::vec3 const look_dir{-glm::sin(angle.x), 0.0f, glm::cos(angle.x)};
        glm::vec3 const side = glm::cross(getUpDirection(), look_dir);
        return glm::rotate(look_dir, angle.y, side);
    }
    /** Get view matrix. */
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(pos, pos + getLookDirection(), getUpDirection());
    }


    /** ImGui Camera config menu. */
    void imgui()
    {
        // Position
        float pos_[3] = {pos.x, pos.y, pos.z};
        ImGui::DragFloat3("Pos", pos_, 0.01f);
        pos.x = pos_[0];
        pos.y = pos_[1];
        pos.z = pos_[2];
        // Rotation
        float angle_[2] = {glm::degrees(angle.x), glm::degrees(angle.y)};
        ImGui::DragFloat2("Angle", angle_, 0.5f, FLT_MIN, FLT_MAX);
        angle.x = fmod(glm::radians(angle_[0]), glm::radians(360.0f));
        angle.y = glm::clamp(
            glm::radians(angle_[1]), -glm::radians(89.0f), glm::radians(89.0f));
        // FOV
        ImGui::SliderFloat("FOV", &fov, min_fov, max_fov);
        // Speed
        ImGui::DragFloat("Speed", &speed, 0.1f, 0.0f, FLT_MAX);
    }
};

#endif
