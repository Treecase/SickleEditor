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
#include <set>


namespace ImGui
{
    /** Recursively draw a directory structure with ImGui TreeNodes. */
    bool DirectoryTree(
        std::filesystem::path const &path, std::filesystem::path *selected,
        std::function<bool(std::filesystem::path)> const &filter=
            [](std::filesystem::path const &){return true;})
    {
        // There are two things we want from our display here--entries must be
        // sorted, and directories must be at the top. To achieve this, we use
        // two std::sets (which sort automatically). One has directories, one
        // with regular files. Once these are populated, we simply display the
        // directories before the files.
        std::set<std::filesystem::path> subdirs{};
        std::set<std::filesystem::path> files{};
        bool result = false;
        for (auto const &entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                subdirs.insert(entry.path());
            }
            else if (entry.is_regular_file() && filter(entry.path()))
            {
                files.insert(entry.path());
            }
        }
        for (auto const &subdir : subdirs)
        {
            if (TreeNode(subdir.filename().string().c_str()))
            {
                result = result || DirectoryTree(subdir, selected, filter);
                TreePop();
            }
        }
        for (auto const &file : files)
        {
            if (ImGui::Selectable(file.filename().string().c_str()))
            {
                *selected = file;
                result = true;
            }
        }
        return result;
    }
}

#endif
