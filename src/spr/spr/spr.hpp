/**
 * spr.hpp - .spr format.
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

#ifndef SE_SPR_HPP
#define SE_SPR_HPP

#include <cstdint>
#include <stdexcept>
#include <array>
#include <vector>


namespace SPR
{
    /**
     * Sprite orientation type.
     *
     * Defines how the sprite rotates with the camera.
     */
    enum Type
    {
        /// Rotates to face camera, but vertical axis is locked.
        VP_PARALLEL_UPRIGHT,
        /// Same as VP_PARALLEL_UPRIGHT, but facing player origin instead of the
        /// camera.
        FACING_UPRIGHT,
        /// Rotates to face camera.
        VP_PARALLEL,
        /// Does not rotate to camera; instead has a fixed orientation defined
        /// in the map data.
        ORIENTED,
        /// Same as VP_PARALLEL but with a rotation defined in map data.
        VP_PARALLEL_ORIENTED,
        /// The number of values in this enum.
        NUM_TYPES
    };

    /**
     * Texture format.
     *
     * Defines sprite transparency information.
     */
    enum TextureFormat
    {
        /// Opaque image with no transparency.
        SPR_NORMAL,
        /// Additive transparency. The alpha value is determined by overall
        /// color value, so eg. black is fully transparent and white is fully
        /// opaque. (Is this correct?)
        SPR_ADDITIVE,
        /// Palette is all the same color, but with transparency determined by
        /// palette index, eg. palette[0] is fully transparent, palette[255] is
        /// fully opaque.
        SPR_INDEXALPHA,
        /// Palette[255] is fully transparent, everything else is opaque.
        SPR_ALPHTEST,
        /// The number of values in this enum.
        NUM_TEXTUREFORMATS
    };

    /**
     * Not sure what this is for, seems like its just left over from Quake.
     */
    enum SyncType
    {
        SYNCHRONIZED,
        RANDOM,
        /// The number of values in this enum.
        NUM_SYNCTYPES
    };

    /**
     * The sprite header.
     */
    struct Header
    {
        /// SPR format version. Half-Life uses version 2.
        uint32_t version;
        /// Sprite orientation.
        Type type;
        /// Texture format.
        TextureFormat format;
        /// Minimum radius of a circle needed to contain the sprite.
        /// = sqrt((max_width/2)^2 + (max_height/2)^2)
        float bounding_radius;
        /// Width/height of the largest frame.
        uint32_t max_width, max_height;
        /// Number of frames in the sprite.
        uint32_t frame_count;
        /// Not sure what this is for. Seems to be left over from Quake.
        float beam_len;
        /// Synchronization type.
        SyncType sync_type;
    };

    /**
     * An RGBA color value.
     */
    struct Color
    {
        uint8_t r, g, b, a;
    };

    /**
     * Sprite palette.
     */
    struct Palette
    {
        /// Number of colors in the palette. Maximum value is 256, despite
        /// being a 16-bit number.
        uint16_t size;
        /// Palette colors.
        std::array<Color, 256> colors;
    };

    /**
     * A single frame of sprite data.
     */
    struct Frame
    {
        /// Not sure if this is used for anything. Left over from Quake.
        uint32_t group;
        /// Centerpoint of the image. The origin is at bottom-right. Positive y
        /// points up, Positive x points right.
        int32_t x, y;
        /// Width/height of the frame.
        uint32_t w, h;
        /// Indexed pixel data.
        uint8_t *data;
    };

    /**
     * Sprite data.
     */
    struct Sprite
    {
        Header header;
        Palette palette;
        std::vector<Frame> frames;
    };


    /** General sprite load error. */
    struct LoadError : public std::runtime_error
    {
        LoadError(std::string const &what): std::runtime_error{what} {}
    };

    /** File doesn't start with "IDSP". */
    struct InvalidMagicNumber : public LoadError
    {
        InvalidMagicNumber(): LoadError{"sprite data must begin with IDSP"} {}
    };

    /** The sprite isn't using version 2. */
    struct InvalidVersion : public LoadError
    {
        InvalidVersion(): LoadError{"sprite must be version 2"} {}
    };


    /**
     * Abstract interface for sprite data input.
     *
     * This interface acts as an adaptor between the end-user and the library.
     * Inherit from this class, implement the methods, and pass that object
     * into the load_sprite function.
     *
     * !!! The .SPR format is little-endian!
     */
    class SpriteStream
    {
    public:
        /**
         * Read a single byte from the input.
         *
         * @return The byte read.
         */
        virtual uint8_t read_byte()=0;
        /**
         * Read an unsigned 16-bit integer from the input.
         * @note .spr format is little-endian.
         *
         * @return The value read.
         */
        virtual uint16_t read_uint16()=0;
        /**
         * Read a signed 32-bit integer from the input.
         * @note .spr format is little-endian.
         *
         * @return The value read.
         */
        virtual int32_t read_int32()=0;
        /**
         * Read an unsigned 32-bit integer from the input.
         * @note .spr format is little-endian.
         *
         * @return The value read.
         */
        virtual uint32_t read_uint32()=0;
        /**
         * Read a 32-bit floating-point number from the input.
         *
         * @return The value read.
         */
        virtual float read_float()=0;
        /**
         * Read COUNT bytes from the input, storing them in a newly-allocated
         * buffer. Caller takes ownership of the buffer.
         *
         * @param count Number of bytes to be read.
         * @return Newly allocated buffer containing the COUNT bytes read.
         */
        virtual uint8_t *read_bytes(size_t count)=0;
    };


    /**
     * Read sprite data from an input stream.
     *
     * @param stream The data stream to read from.
     * @return The resulting sprite.
     * @throw SPR::LoadError if the data is invalid.
     */
    Sprite load_sprite(SpriteStream &stream);
}

#endif
