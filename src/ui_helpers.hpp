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
#include <string>


namespace ImGui
{
    /** Recursively draw a directory structure with ImGui TreeNodes. */
    bool DirectoryTree(
        std::string path, std::filesystem::path *selected,
        std::function<bool(std::filesystem::path)> filter)
    {
        bool result = false;
        for (auto entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                if (TreeNode(entry.path().filename().string().c_str()))
                {
                    DirectoryTree(entry.path().string(), selected, filter);
                    TreePop();
                }
            }
            else if (entry.is_regular_file() && filter(entry.path()))
            {
                if (ImGui::Selectable(entry.path().filename().string().c_str()))
                {
                    *selected = entry.path().string();
                    result = true;
                }
            }
        }
        return result;
    }
    bool DirectoryTree(std::string path, std::filesystem::path *selected)
    {
        return DirectoryTree(
            path,
            selected,
            [](std::filesystem::path p){return true;});
    }
}

#endif
