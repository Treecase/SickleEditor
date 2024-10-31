/**
 * WADInputStream.hpp - Abstract interface for reading WAD data.
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

#ifndef SE_FILES_WAD_WADINPUTSTREAM_HPP
#define SE_FILES_WAD_WADINPUTSTREAM_HPP

#include <cstddef>
#include <cstdint>

namespace WAD
{
    /**
     * Abstract class for an input stream used to read WAD data.
     */
    struct WADInputStream
    {
        /**
         * Seek the input stream to offset.
         *
         * @param offset Position of the stream after seeking.
         */
        virtual void seek(size_t offset) = 0;

        /**
         * Read exactly count bytes from the stream into buf.
         *
         * @param buf Buffer at least count bytes long.
         * @param count Number of bytes to read into buffer.
         */
        virtual void read_bytes(void *buf, size_t count) = 0;

        /**
         * Read an unsigned 8-bit integer.
         *
         * @return Unsigned 8-bit integer.
         */
        virtual uint8_t read_uint8() = 0;

        /**
         * Read an unsigned 32-bit integer.
         *
         * @return Unsigned 32-bit integer.
         * @note WAD format is little-endian, this method returns host-endian.
         */
        virtual uint32_t read_uint32() = 0;
    };
} // namespace WAD

#endif
