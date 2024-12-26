/**
 * OrbitCam.hpp - Orbiting camera.
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

#ifndef SE_ORBITCAM_HPP
#define SE_ORBITCAM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

/** Orbiting Camera. */
class OrbitCam
{
public:
    glm::vec2 angle;       // x/y angle
    float zoom;            // Distance from origin
    float fov;             // FOV
    float min_zoom = 0.5f; // Distance from origin
    float min_fov = 30.0f, // FOV min value
        max_fov = 90.0f;   // FOV max value

    OrbitCam()
    : angle{0.0f, 0.0f}
    , zoom{2.0f}
    , fov{70.0f}
    {
    }

    /** Set camera angle. */
    void setAngle(glm::vec2 value)
    {
        angle = glm::mod(value, glm::radians(360.0f));
    }

    /** Set the camera zoom. */
    void setZoom(float zoom_) { zoom = glm::max(zoom_, min_zoom); }

    /** Set the camera FOV. */
    void setFOV(float fov_) { fov = glm::clamp(fov_, min_fov, max_fov); }

    /** Turn the camera by `delta` degrees. */
    void rotate(glm::vec2 delta) { setAngle(angle + glm::radians(delta)); }

    /** Get view matrix. */
    glm::mat4 getViewMatrix() const
    {
        glm::vec3 const pos{0.0f, 0.0f, -zoom};
        glm::vec3 const up{0.0f, 1.0f, 0.0f};
        return glm::rotate(
            glm::rotate(
                glm::lookAt(pos, glm::vec3{0.0f}, up),
                angle.y,
                glm::cross(up, pos)),
            angle.x,
            up);
    }
};

#endif
