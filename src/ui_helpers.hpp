/**
 * ui_helpers.hpp - Helper functions for ImGUI.
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

#ifndef _UI_HELPERS_HPP
#define _UI_HELPERS_HPP

#include <imgui.h>

#include <filesystem>
#include <functional>


namespace ImGui
{
    /** Recursively draw a directory structure with ImGui TreeNodes. */
    bool DirectoryTree(
        std::filesystem::path const &path, std::filesystem::path *selected,
        std::function<bool(std::filesystem::path)> const &filter=
            [](std::filesystem::path const &){return true;});

    /** File picker popup modal. Returns true when the path has been updated. */
    bool FilePicker(char const *name, std::filesystem::path *path);
}

#endif
