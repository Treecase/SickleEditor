/**
 * Texture.hpp - World3D::Texture class.
 * Copyright (C) 2023-2024 Trevor Last
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

#include <editor/textures/TextureInfo.hpp>
#include <glutils/glutils.hpp>

#include <memory>

namespace World3D
{
    class Texture
    {
    public:
        std::shared_ptr<GLUtil::Texture> texture{nullptr};
        unsigned int width, height;

        /**
         * Get the "Missing Texture" texture. It will only be generated once,
         * and will be reused on subsequent calls.
         *
         * @warning Requires an active OpenGL context.
         */
        static std::shared_ptr<Texture> make_missing_texture();

        /**
         * Create a texture object for the named texture.
         *
         * @warning Requires an active OpenGL context.
         */
        static std::shared_ptr<Texture> create_for_name(
            std::string const &texture_name);

    protected:
        using TexInfo = std::shared_ptr<Sickle::Editor::Textures::TextureInfo>;

        static decltype(texture) make_gltexture_for_texinfo(
            TexInfo const &texinfo);

        Texture() = default;
        /** @warning Requires an active OpenGL context. */
        Texture(TexInfo const &texinfo);
    };
} // namespace World3D

#endif
