/**
 * spr.cpp - .spr format.
 * Copyright (C) 2024 Trevor Last
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

// The isn't a ton of information about the sprite format online.
// This code is based on info from:
// - https://github.com/yuraj11/HL-Texture-Tools
// - https://developer.valvesoftware.com/wiki/SPR

#include "spr.hpp"

#include <cstring>


static SPR::Header load_header(SPR::SpriteStream &stream)
{
    SPR::Header header{};
    header.version = stream.read_uint32();
    if (header.version != 2)
        throw SPR::InvalidVersion{};
    header.type = static_cast<SPR::Type>(stream.read_uint32());
    header.format = static_cast<SPR::TextureFormat>(stream.read_uint32());
    header.bounding_radius = stream.read_float();
    header.max_width = stream.read_uint32();
    header.max_height = stream.read_uint32();
    header.frame_count = stream.read_uint32();
    header.beam_len = stream.read_float();
    header.sync_type = static_cast<SPR::SyncType>(stream.read_uint32());
    return header;
}

static SPR::Palette load_palette(
    SPR::SpriteStream &stream,
    SPR::TextureFormat format)
{
    SPR::Palette palette{};
    palette.size = stream.read_uint16();
    if (palette.size > 256)
        throw SPR::LoadError{"invalid palette size"};
    for (uint16_t i = 0; i < palette.size; ++i)
    {
        SPR::Color color{};
        color.r = stream.read_byte();
        color.g = stream.read_byte();
        color.b = stream.read_byte();
        switch (format)
        {
        case SPR::TextureFormat::SPR_INDEXALPHA:
            color.a = i;
            break;
        case SPR::TextureFormat::SPR_ALPHTEST:
            color.a = (i == 0xff ? 0x00 : 0xff);
            break;
        case SPR::TextureFormat::SPR_ADDITIVE:
            color.a = (color.r + color.g + color.b) / 3;
            break;
        default:
            color.a = 0xff;
            break;
        }
        palette.colors.at(i) = color;
    }
    return palette;
}

static std::vector<SPR::Frame> load_frames(
    SPR::SpriteStream &stream,
    uint32_t frame_count)
{
    std::vector<SPR::Frame> frames{};
    for (uint32_t i = 0; i < frame_count; ++i)
    {
        SPR::Frame frame{};
        frame.group = stream.read_uint32();
        frame.x = stream.read_int32();
        frame.y = stream.read_int32();
        frame.w = stream.read_uint32();
        frame.h = stream.read_uint32();
        frame.data = stream.read_bytes(frame.w * frame.h);
        frames.push_back(frame);
    }
    return frames;
}


SPR::Sprite SPR::load_sprite(SpriteStream &stream)
{
    char magic[4];
    for (size_t i = 0; i < 4; ++i)
        magic[i] = stream.read_byte();
    if (memcmp(magic, "IDSP", 4) != 0)
        throw InvalidMagicNumber{};

    Sprite sprite{};
    sprite.header = load_header(stream);
    sprite.palette = load_palette(stream, sprite.header.format);
    sprite.frames = load_frames(stream, sprite.header.frame_count);

    return sprite;
}
