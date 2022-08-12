/**
 * mdl2gl.hpp - Convert loaded MDL data into OpenGL objects.
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

#ifndef _MDL2GL_HPP
#define _MDL2GL_HPP

#include "glUtils/glUtil.hpp"
#include "load_model.hpp"


/** Convert from MDL texture format to an OpenGL texture object. */
GLUtil::Texture texture2GLTexture(MDL::Texture const &texture);

#endif
