/**
 * mdl2gl.cpp - Convert loaded MDL data into OpenGL objects.
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

#include "mdl2gl.hpp"


template<typename T> T min(T a, T b) {return a < b? a : b;}


GLUtil::Texture texture2GLTexture(MDL::Texture const &texture)
{
    // Calculate resized power-of-two dimensions.
    int resized_w = texture.w > 256? 256 : 1;
    int resized_h = texture.h > 256? 256 : 1;
    for (; resized_w < texture.w && resized_w < 256; resized_w *= 2)
        ;
    for (; resized_h < texture.h && resized_h < 256; resized_h *= 2)
        ;

    // Generate the resized texture.
    auto unpaletted = new GLubyte[resized_w * resized_h * 4];
    for (int y = 0; y < resized_h; ++y)
    {
        for (int x = 0; x < resized_w; ++x)
        {
            // TODO: clean this up
            // The x,y coordinates of the samples.
            int x1 = min(((x+0.25)/(float)resized_w)*texture.w, texture.w-1.0);
            int x2 = min(((x+0.75)/(float)resized_w)*texture.w, texture.w-1.0);
            int y1 = min(((y+0.25)/(float)resized_h)*texture.h, texture.h-1.0);
            int y2 = min(((y+0.75)/(float)resized_h)*texture.h, texture.h-1.0);
            // RGB triples of the sampled pixels.
            std::array<uint8_t, 3> samples[4] = {
                texture.palette[texture.data[y1*texture.w + x1]],
                texture.palette[texture.data[y1*texture.w + x2]],
                texture.palette[texture.data[y2*texture.w + x1]],
                texture.palette[texture.data[y2*texture.w + x2]]
            };
            // Averaged RGB value of the samples.
            int averages[3] = {
                (samples[0][0]+samples[1][0]+samples[2][0]+samples[3][0])/4,
                (samples[0][1]+samples[1][1]+samples[2][1]+samples[3][1])/4,
                (samples[0][2]+samples[1][2]+samples[2][2]+samples[3][2])/4,
            };
            int offset = (y * resized_w + x) * 4;
            unpaletted[offset+0] = averages[0];
            unpaletted[offset+1] = averages[1];
            unpaletted[offset+2] = averages[2];
            unpaletted[offset+3] = 0xFF;
        }
    }

    // Create the GL texture.
    GLUtil::Texture t{GL_TEXTURE_2D, texture.name};
    t.bind();
    t.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        t.type(), 0, GL_RGBA, resized_w, resized_h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, unpaletted);
    delete[] unpaletted;
    t.unbind();
    return t;
}
