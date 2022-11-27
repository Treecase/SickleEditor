/**
 * Transform.hpp - 3D transform.
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

#ifndef _TRANSFORM_HPP
#define _TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


/** 3D transform. */
struct Transform
{
    // Model translation.
    glm::vec3 translation;
    // Model rotation.
    glm::vec3 rotation;
    // Model scaling.
    glm::vec3 scale;


    Transform()
    :   translation{0.0f, 0.0f, 0.0f}
    ,   rotation{0.0f, 0.0f, 0.0f}
    ,   scale{1.0f, 1.0f, 1.0f}
    {
    }

    Transform(glm::vec3 const &translation, glm::vec3 const &rotation, glm::vec3 const &scale)
    :   translation{translation}
    ,   rotation{rotation}
    ,   scale{scale}
    {
    }


    /** Get transform matrix. */
    glm::mat4 getMatrix() const
    {
        // TODO: rotation's a bit weird?
        return
            glm::rotate(
                glm::rotate(
                    glm::rotate(
                        glm::scale(
                            glm::translate(
                                glm::identity<glm::mat4>(),
                                translation
                            ),
                            scale
                        ),
                        rotation.y,
                        glm::vec3{0.0f, 1.0f, 0.0f}
                    ),
                    rotation.z,
                    glm::vec3{0.0f, 0.0f, 1.0f}
                ),
                rotation.x,
                glm::vec3{1.0f, 0.0f, 0.0f}
            );
    }
};

#endif
