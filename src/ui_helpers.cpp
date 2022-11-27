/**
 * ui_helpers.cpp - Helper functions for ImGUI.
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

#include "ui_helpers.hpp"

#include <misc/cpp/imgui_stdlib.h>

#include <set>


bool allPass(std::filesystem::directory_entry const &)
{
    return true;
}
bool isDirectory(std::filesystem::directory_entry const &e)
{
    return e.is_directory();
}
bool isRegularFile(std::filesystem::directory_entry const &e)
{
    return e.is_regular_file();
}

std::function<bool(std::filesystem::directory_entry const &)>
isRegularFileWithFilter(
    std::function<bool(std::filesystem::path)> const &filter)
{
    return [&filter](auto e){return isRegularFile(e) && filter(e.path());};
}



/**
 * Get a sorted iterable containing all files in directory `path` for which
 * `filter` returns true.
 */
auto getDirectoryElements(
    std::filesystem::path const &path,
    std::function<bool(std::filesystem::directory_entry const &)> const &
        filter=allPass)
{
    std::set<std::filesystem::path> paths{};
    for (auto const &entry : std::filesystem::directory_iterator(path))
        if (filter(entry))
            paths.insert(entry.path());
    return paths;
}


bool ImGui::DirectoryTree(
    std::filesystem::path const &path, std::filesystem::path *selected,
    std::function<bool(std::filesystem::path)> const &filter)
{
    // There are two things we want from our display here--entries must be
    // sorted, and directories must be at the top. To achieve this, we use
    // two std::sets (which sort automatically). One has directories, one
    // with regular files. Once these are populated, we simply display the
    // directories before the files.
    auto subdirs = getDirectoryElements(path, isDirectory);
    auto files = getDirectoryElements(path, isRegularFileWithFilter(filter));
    bool result = false;
    for (auto const &subdir : subdirs)
    {
        if (ImGui::TreeNode(subdir.filename().string().c_str()))
        {
            result = result || ImGui::DirectoryTree(subdir, selected, filter);
            ImGui::TreePop();
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

bool ImGui::FilePicker(char const *name, std::filesystem::path *path)
{
    // TODO: clean this up (get rid of static vars)
    static bool reopened = true;
    static std::filesystem::path p = *path;
    if (reopened)
    {
        p = *path;
        reopened = false;
    }
    bool r = false;

    if (ImGui::BeginPopupModal(name))
    {
        std::string pathstr{p.string()};
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##Path", &pathstr);
        p = pathstr;

        if (ImGui::BeginChild(
            "File List", ImVec2(0, -GetFrameHeightWithSpacing()), true))
        {
            if (ImGui::Selectable(".."))
                p = p.parent_path();
            for (auto const &dir : getDirectoryElements(p, isDirectory))
                if (ImGui::Selectable(dir.filename().string().c_str()))
                    p = dir;
        }
        ImGui::EndChild();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            reopened = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Select"))
        {
            *path = p;
            r = true;
            ImGui::CloseCurrentPopup();
            reopened = true;
        }
        ImGui::EndPopup();
    }
    return r;
}

bool ImGui::Transform(::Transform *transform, bool uniform_scale)
{
    bool changed = false;
    if (ImGui::Button("Reset"))
    {
        transform->translation = glm::vec3{0.0f};
        transform->rotation = glm::vec3{0.0f};
        transform->scale = glm::vec3{1.0f};
        changed = true;
    }
    auto const rd = glm::degrees(transform->rotation);
    float t[3] = {transform->translation.x, transform->translation.y, transform->translation.z};
    float r[3] = {rd.x, rd.y, rd.z};
    float s[3] = {transform->scale.x, transform->scale.y, transform->scale.z};
    if (ImGui::DragFloat3("Translation", t, 0.01f))
    {
        transform->translation = glm::vec3{t[0], t[1], t[2]};
        changed = true;
    }
    if (ImGui::DragFloat3("Rotation", r, 0.5f))
    {
        glm::vec3 const rv{r[0], r[1], r[2]};
        transform->rotation = glm::mod(glm::radians(rv), glm::radians(360.0f));
        changed = true;
    }
    if (uniform_scale)
    {
        if (ImGui::DragFloat("Scale", s, 0.005f, FLT_MIN, FLT_MAX))
        {
            transform->scale = glm::vec3{s[0]};
            changed = true;
        }
    }
    else
    {
        if (ImGui::DragFloat3("Scale", s, 0.005f, FLT_MIN, FLT_MAX))
        {
            transform->scale = glm::vec3{s[0], s[1], s[2]};
            changed = true;
        }
    }
    return changed;
}

void ImGui::FreeCam(::FreeCam *cam)
{
    // Position
    float pos_[3] = {cam->pos.x, cam->pos.y, cam->pos.z};
    ImGui::DragFloat3("Pos", pos_, 0.01f);
    cam->pos = glm::vec3{pos_[0], pos_[1], pos_[2]};
    // Rotation
    float angle_[2] = {
        glm::degrees(cam->angle.x), glm::degrees(cam->angle.y)};
    ImGui::DragFloat2("Angle", angle_, 0.5f, FLT_MIN, FLT_MAX);
    cam->angle.x = fmod(glm::radians(angle_[0]), glm::radians(360.0f));
    cam->angle.y = glm::clamp(
        glm::radians(angle_[1]), -glm::radians(89.0f), glm::radians(89.0f));
    // FOV
    ImGui::SliderFloat("FOV", &cam->fov, cam->min_fov, cam->max_fov);
    // Speed
    ImGui::DragFloat("Speed", &cam->speed, 0.1f, 0.0f, FLT_MAX);
}
