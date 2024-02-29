/**
 * Texture.cpp - World3D::Texture class.
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

#include "Texture.hpp"


/** Convert WAD lump mipmap to RGBA format. */
auto
mipmap_to_rgba(WAD::TexLump const &lump, std::vector<uint8_t> const &mipmap)
{
    std::vector<uint8_t> rgba{};
    auto const &palette = lump.palette();
    for (auto const &palette_index : mipmap)
    {
        auto const &rgb = palette.at(palette_index);
        rgba.insert(rgba.end(), rgb.cbegin(), rgb.cend());
        rgba.push_back(0xff);
    }
    return rgba;
}


/** Convert paletted texture data to RGBA format. */
auto texlump_to_rgba(WAD::TexLump const &lump)
{
    std::vector<std::vector<uint8_t>> rgba_mipmaps{};
    std::vector const mipmaps{
        lump.tex1(), lump.tex2(), lump.tex4(), lump.tex8()};
    for (auto const &mipmap : mipmaps)
        rgba_mipmaps.emplace_back(mipmap_to_rgba(lump, mipmap));
    return rgba_mipmaps;
}


/** Create a GLUtil::Texture shared_ptr. */
auto make_texture(std::string const &name)
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


/** Make a GL Texture from a WAD lump. */
auto texture_from_lump(WAD::TexLump const &texlump)
{
    auto const texture = make_texture(texlump.name());
    auto const mipmaps = texlump_to_rgba(texlump);
    for (size_t mipmap = 0; mipmap < mipmaps.size(); ++mipmap)
    {
        GLsizei const scale = pow(2, mipmap);
        glTexImage2D(
            texture->type(), mipmap, GL_RGBA,
            texlump.width() / scale, texlump.height() / scale, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, mipmaps.at(mipmap).data());
    }
    texture->unbind();
    return texture;
}



World3D::Texture::Texture(WAD::TexLump const &texlump)
:   texture{texture_from_lump(texlump)}
,   width{static_cast<int>(texlump.width())}
,   height{static_cast<int>(texlump.height())}
{
}


World3D::Texture World3D::Texture::make_missing_texture()
{
    static Texture missing{};
    // Resuse the existing texture if its already been generated.
    if (missing.texture)
        return missing;

    constexpr int SIZE = 128;
    constexpr int HSIZE = 0.5 * SIZE;
    missing.width = SIZE;
    missing.height = SIZE;
    missing.texture = make_texture("MISSING");
    uint8_t pixels[SIZE*SIZE * 4];
    for (size_t y = 0; y < SIZE; ++y)
    {
        for (size_t x = 0; x < SIZE; ++x)
        {
            auto const idx = 4 * (y * SIZE + x);
            if (x < HSIZE && y < HSIZE || x >= HSIZE && y >= HSIZE)
            {
                pixels[idx+0] = 0x00;
                pixels[idx+1] = 0x00;
                pixels[idx+2] = 0x00;
            }
            else
            {
                pixels[idx+0] = 0xff;
                pixels[idx+1] = 0x00;
                pixels[idx+2] = 0xff;
            }
            pixels[idx+3] = 0xff;
        }
    }
    glTexImage2D(
        missing.texture->type(), 0, GL_RGBA,
        missing.width, missing.height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(missing.texture->type());
    return missing;
}
