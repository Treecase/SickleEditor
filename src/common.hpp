/**
 * common.hpp - Sickle common data.
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

#ifndef SE_COMMON_HPP
#define SE_COMMON_HPP

#include <filesystem>


/** App config. */
struct Config
{
    std::filesystem::path game_dir;
    std::filesystem::path game_def;
    std::filesystem::path maps_dir;
    int *window_width,
        *window_height;
    float mouse_sensitivity;
};

#endif
