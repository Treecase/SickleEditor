/**
 * Texture.hpp - World3D::Texture class.
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

#ifndef SE_WORLD3D_TEXTURE_HPP
#define SE_WORLD3D_TEXTURE_HPP

#include "DeferredExec.hpp"

#include <files/wad/lumps.hpp>
#include <glutils/glutils.hpp>

#include <memory>


namespace World3D
{
    struct Texture
    {
        std::shared_ptr<GLUtil::Texture> texture{nullptr};
        int width, height;

        /**
         * Get the "Missing Texture" texture. It will only be generated once,
         * and will be reused on subsequent calls.
         *
         * @warning Requires an active OpenGL context.
         */
        static Texture make_missing_texture();

        Texture()=default;

        /** @warning Requires an active OpenGL context. */
        Texture(WAD::TexLump const &texlump);
    };
}

#endif
