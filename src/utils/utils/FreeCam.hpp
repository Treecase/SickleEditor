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

#ifndef SE_FREECAM_HPP
#define SE_FREECAM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>


/** Freelook first-person camera. */
struct FreeCam
{
    // Position
    glm::vec3 pos{0.0f, 0.0f, 0.0f};
    // x/y angle (radians)
    glm::vec2 angle{0.0f, 0.0f};
    // FOV (degrees)
    float fov{70.0f};
    // Movement speed
    float speed{1.0f};
    // FOV min/max value
    float min_fov{30.0f}, max_fov{90.0f};

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
};

#endif
