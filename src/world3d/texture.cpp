/**
 * texture.cpp - World3D::Texture class.
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


/** Convert WAD lump mipmap to RGBA format. */
auto
mipmap_to_rgba(WAD::TexLump const &lump, std::vector<uint8_t> const &mipmap)
{
    std::vector<uint8_t> rgba{};
    for (auto const &palette_index : mipmap)
    {
        auto const &rgb = lump.palette.at(palette_index);
        rgba.insert(rgba.end(), rgb.cbegin(), rgb.cend());
        rgba.push_back(0xff);
    }
    return rgba;
}


/** Convert paletted texture data to RGBA format. */
auto texlump_to_rgba(WAD::TexLump const &lump)
{
    std::vector<std::vector<uint8_t>> rgba_mipmaps{};
    for (auto const &mipmap : {lump.tex1, lump.tex2, lump.tex4, lump.tex8})
        rgba_mipmaps.emplace_back(mipmap_to_rgba(lump, mipmap));
    return rgba_mipmaps;
}


/** Make a GL Texture from a WAD lump. */
auto texture_from_lump(WAD::TexLump const &texlump)
{
    auto texture = std::make_shared<GLUtil::Texture>(
        GL_TEXTURE_2D, texlump.name);
    texture->bind();
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    texture->setParameter(GL_TEXTURE_MAX_LEVEL, 3);
    auto mipmaps = texlump_to_rgba(texlump);
    for (size_t mipmap = 0; mipmap < mipmaps.size(); ++mipmap)
    {
        GLsizei scale = pow(2, mipmap);
        glTexImage2D(
            texture->type(), mipmap, GL_RGBA,
            texlump.width/scale, texlump.height/scale, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, mipmaps[mipmap].data());
    }
    texture->unbind();
    return texture;
}



World3D::Texture::Texture(WAD::TexLump const &texlump)
:   texture{texture_from_lump(texlump)}
,   width{(int)texlump.width}
,   height{(int)texlump.height}
{
}
