/**
 * vertex.cpp - World3D::Vertex class.
 * Copyright (C) 2023 Trevor Last
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

#include "world3d/world3d.hpp"


World3D::Vertex::Vertex(glm::vec3 pos, glm::vec2 uv)
:   position{pos}
,   uv{uv}
{
}


std::array<GLfloat, World3D::Vertex::ELEMENTS> World3D::Vertex::as_vbo() const
{
    return {position.x, position.y, position.z, uv.x, uv.y};
}
