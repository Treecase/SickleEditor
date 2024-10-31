/**
 * Texture.cpp - World3D::Texture class.
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

#include "Texture.hpp"

#include <editor/textures/TextureManager.hpp>

/** Create a GLUtil::Texture shared_ptr. */
static auto make_texture(std::string const &name)
{
    auto texture = std::make_shared<GLUtil::Texture>(GL_TEXTURE_2D, name);
    texture->bind();
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    texture->setParameter(GL_TEXTURE_MAX_LEVEL, 3);
    return texture;
}

decltype(World3D::Texture::texture) World3D::Texture::
    make_gltexture_for_texinfo(TexInfo const &texinfo)
{
    static std::vector const MIPMAPS{
        Sickle::Editor::Textures::MipmapLevel::MIPMAP_FULL,
        Sickle::Editor::Textures::MipmapLevel::MIPMAP_HALF,
        Sickle::Editor::Textures::MipmapLevel::MIPMAP_QUARTER,
        Sickle::Editor::Textures::MipmapLevel::MIPMAP_EIGHTH,
    };

    auto const texture = make_texture(texinfo->get_name());
    for (auto const mipmap : MIPMAPS)
    {
        glTexImage2D(
            texture->type(),
            static_cast<GLint>(mipmap),
            GL_RGBA,
            texinfo->get_width(mipmap),
            texinfo->get_height(mipmap),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texinfo->load_rgba(mipmap).get());
    }
    texture->unbind();
    return texture;
}

World3D::Texture::Texture(TexInfo const &texinfo)
: texture{make_gltexture_for_texinfo(texinfo)}
, width{texinfo->get_width()}
, height{texinfo->get_height()}
{
}

std::shared_ptr<World3D::Texture> World3D::Texture::make_missing_texture()
{
    static std::shared_ptr<Texture> missing{nullptr};
    // Reuse the existing texture if its already been generated.
    if (missing)
    {
        return missing;
    }

    missing.reset(new Texture{});

    constexpr int SIZE = 128;
    constexpr int HSIZE = 0.5 * SIZE;
    missing->width = SIZE;
    missing->height = SIZE;
    missing->texture = make_texture("MISSING");
    uint8_t pixels[SIZE * SIZE * 4];
    for (size_t y = 0; y < SIZE; ++y)
    {
        for (size_t x = 0; x < SIZE; ++x)
        {
            auto const idx = 4 * (y * SIZE + x);
            if ((x < HSIZE && y < HSIZE) || (x >= HSIZE && y >= HSIZE))
            {
                pixels[idx + 0] = 0x00;
                pixels[idx + 1] = 0x00;
                pixels[idx + 2] = 0x00;
            }
            else
            {
                pixels[idx + 0] = 0xff;
                pixels[idx + 1] = 0x00;
                pixels[idx + 2] = 0xff;
            }
            pixels[idx + 3] = 0xff;
        }
    }
    glTexImage2D(
        missing->texture->type(),
        0,
        GL_RGBA,
        missing->width,
        missing->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels);
    glGenerateMipmap(missing->texture->type());
    return missing;
}

std::shared_ptr<World3D::Texture> World3D::Texture::create_for_name(
    std::string const &texture_name)
{
    auto &texman = Sickle::Editor::Textures::TextureManager::get_reference();
    TexInfo texinfo{nullptr};

    // Get texture info for the named texture.
    try
    {
        texinfo = texman.get_texture(texture_name);
    }
    // If this fails, the texture doesn't exist. Return missing texture.
    catch (std::out_of_range const &)
    {
        return make_missing_texture();
    }

    // Try to get cached texture.
    try
    {
        return texinfo->get_cached<Texture>();
    }
    catch (std::out_of_range const &)
    {
    }

    // Texture isn't cached, we have to create (and cache) it.
    std::shared_ptr<Texture> texture{new Texture{texinfo}};
    texinfo->cache_object(texture);
    return texture;
}
