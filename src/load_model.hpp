/**
 * load_model.hpp - Load .mdl files.
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

#ifndef _LOAD_MODEL
#define _LOAD_MODEL

#include <string>
#include <vector>

#include <cstdint>


namespace MDL
{
/** MDL Texture data. */
struct Texture
{
    std::string name;
    int w, h;
    uint8_t *data;
    uint8_t palette[256 * 3];
};

/** Loaded MDL data. */
struct Model
{
    std::string name;
    std::vector<Texture> textures;
};


/** Load a .mdl file. */
Model load_mdl(std::string const &path);
}

#endif
