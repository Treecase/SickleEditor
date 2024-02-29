/**
 * rmf.hpp - Rich Map Format data.
 * Copyright (C) 2023 Trevor Last
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

#ifndef SE_RMF_HPP
#define SE_RMF_HPP

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


namespace RMF
{
    struct LoadError : public std::runtime_error
    {
        LoadError(std::streampos where, std::string const &what)
        :   std::runtime_error{_make_what(where, what)}
        {
        }

    private:
        static std::string
        _make_what(std::streampos where, std::string const &what);
    };


    struct Color
    {
        uint8_t r, g, b;
    };

    struct VisGroup
    {
        std::string name{};
        Color color{};
        int index;
        bool visible;
    };

    struct Vector
    {
        float x, y, z;
    };

    struct Face
    {
        std::string texture_name{};
        Vector texture_u{};
        float texture_x_shift;
        Vector texture_v{};
        float texture_y_shift;
        float texture_rotation;
        float texture_x_scale, texture_y_scale;
        std::vector<Vector> vertices{};
        Vector plane[3];
    };

    struct Object
    {
        int visgroup_index;
        Color color{};
    };

    struct Solid : Object
    {
        std::vector<Face> faces{};
    };

    struct Entity : Object
    {
        std::vector<Solid> brushes{};
        std::string classname{};
        int flags;
        std::unordered_map<std::string, std::string> kv_pairs{};
        Vector position{};
    };

    struct Group : Object
    {
        std::vector<Solid> brushes{};
        std::vector<Entity> entities{};
        std::vector<Group> groups{};
    };

    struct Corner
    {
        Vector position{};
        int index;
        std::string name_override{};
        std::unordered_map<std::string, std::string> kv_pairs{};
    };

    struct Path
    {
        std::string name{};
        std::string class_{};
        int type;
        std::vector<Corner> corners{};
    };

    struct Camera
    {
        Vector eye{};
        Vector look{};
    };


    struct RichMap
    {
        float version;
        std::vector<VisGroup> visgroups{};
        Group objects{};
        std::string worldspawn_name{};
        std::unordered_map<std::string, std::string> worldspawn_properties{};
        std::vector<Path> paths{};
        int active_camera;
        std::vector<Camera> cameras{};
    };

    RichMap load(std::string const &path);
}

#endif
