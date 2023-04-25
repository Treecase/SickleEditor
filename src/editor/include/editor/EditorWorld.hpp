/**
 * EditorWorld.hpp - Editor World class.
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

#ifndef SE_EDITORWORLD_HPP
#define SE_EDITORWORLD_HPP

#include <map/map.hpp>
#include <rmf/rmf.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sigc++/signal.h>

#include <stack>
#include <unordered_map>


namespace Sickle
{
    template<typename T>
    class Property
    {
    public:
        Property()=default;
        Property(T init): _value{init} {}
        auto &signal_changed() {return _signal_changed;}
        void set(T value) {_value = value; signal_changed().emit();}
        T operator=(T value) {set(value); return _value;}
        operator T() const {return _value;}
        T get() const {return _value;}
    private:
        T _value{};
        sigc::signal<void()> _signal_changed{};
    };
}


namespace Sickle
{
namespace Editor
{
    class Face
    {
    public:
        std::vector<glm::vec3> vertices{};
        std::string texture{};
        glm::vec3 u{0.0f}, v{0.0f};
        glm::vec2 shift{0.0f, 0.0f};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation{0.0f};

        // Sorted same direction as VERTICES list.
        std::array<glm::vec3, 3> get_plane_points() const
        {
            return {vertices.at(0), vertices.at(1), vertices.at(2)};
        }

        Face(
            std::vector<glm::vec3> vertices,
            std::string texture,
            glm::vec3 u,
            glm::vec3 v,
            glm::vec2 shift,
            glm::vec2 scale,
            float rotation)
        :   vertices{vertices}
        ,   texture{texture}
        ,   u{u}
        ,   v{v}
        ,   shift{shift}
        ,   scale{scale}
        ,   rotation{rotation}
        {
            if (vertices.size() < 3)
                throw std::runtime_error{"not enough points for a face"};
        }

        operator MAP::Plane() const
        {
            auto abc = get_plane_points();
            MAP::Plane out{
                abc[0], abc[1], abc[2],
                vertices,
                texture,
                u, v,
                shift,
                rotation,
                scale
            };
            return out;
        }
    };

    /** Editor Brush interface. */
    class Brush
    {
    public:
        Property<bool> is_selected{false};
        std::vector<Face> faces{};

        auto &signal_changed() {return _signal_changed;}

        void transform(glm::mat4 const &matrix)
        {
            for (auto &face : faces)
                for (auto &vertex : face.vertices)
                    vertex = glm::vec3{matrix * glm::vec4{vertex, 1.0}};
            signal_changed().emit();
        }

        void translate(glm::vec3 const &translation)
        {
            transform(glm::translate(glm::mat4{1.0}, translation));
        }

        Brush(MAP::Brush const &brush)
        {
            for (auto const &plane : brush.planes)
            {
                faces.emplace_back(
                    plane.vertices,
                    plane.miptex,
                    plane.s, plane.t,
                    plane.offsets,
                    plane.scale,
                    plane.rotation);
            }
        }

        Brush(RMF::Solid const &solid)
        {
            for (auto const &face : solid.faces)
            {
                std::vector<glm::vec3> verts{};
                for (auto const &vert : face.vertices)
                    verts.emplace(verts.begin(), vert.x, vert.y, vert.z);
                faces.emplace_back(
                    verts,
                    face.texture_name,
                    glm::vec3{
                        face.texture_u.x, face.texture_u.y, face.texture_u.z},
                    glm::vec3{
                        face.texture_v.x, face.texture_v.y, face.texture_v.z},
                    glm::vec2{face.texture_x_shift, face.texture_y_shift},
                    glm::vec2{face.texture_x_scale, face.texture_y_scale},
                    face.texture_rotation);
            }
        }

        // TEMP
        operator MAP::Brush() const
        {
            MAP::Brush out{};
            for (auto const &face : faces)
                out.planes.push_back(face);
            return out;
        }

    private:
        sigc::signal<void()> _signal_changed{};

        // TODO:
        // - visgroup id
        // - color
        // ^ these are only used by worldspawn brushes?
    };

    class Entity
    {
    public:
        std::unordered_map<std::string, std::string> properties{};
        std::vector<Brush> brushes{};

        Entity()=default;

        Entity(MAP::Entity const &entity)
        :   properties{entity.properties}
        {
            for (auto const &brush : entity.brushes)
                brushes.push_back(brush);
        }

        Entity(RMF::Entity const &entity)
        :   properties{entity.kv_pairs}
        {
            properties["classname"] = entity.classname;
            for (auto const &brush : entity.brushes)
                brushes.push_back(brush);
        }

        // TEMP
        operator MAP::Entity() const
        {
            MAP::Entity out{};
            out.properties = properties;
            for (auto const &brush : brushes)
                out.brushes.push_back(brush);
            return out;
        }

        // TODO:
        // - visgroup id
        // - color
    };

    class Map
    {
    public:
        std::vector<Entity> entities{};

        Map()=default;

        Map(MAP::Map const &map)
        {
            for (auto const &entity : map.entities)
                entities.push_back(entity);
        }

        Map(RMF::RichMap const &map)
        {
            Entity worldspawn{};
            worldspawn.properties = map.worldspawn_properties;
            worldspawn.properties["classname"] = map.worldspawn_name;
            // TEMP -- needed by GLMap
#if WIN32
            worldspawn.properties["wad"] = (
                "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/decals.wad;"
                "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/halflife.wad;"
                "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/liquids.wad;"
                "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/spraypaint.wad;"
                "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/xeno.wad"
            );
#else
            std::string home{getenv("HOME")};
            worldspawn.properties["wad"] = (
                home + "/.steam/steam/steamapps/common/Half-Life SDK/Texture Wad Files/decals.wad;"
                + home + "/.steam/steam/steamapps/common/Half-Life SDK/Texture Wad Files/halflife.wad;"
                + home + "/.steam/steam/steamapps/common/Half-Life SDK/Texture Wad Files/liquids.wad;"
                + home + "/.steam/steam/steamapps/common/Half-Life SDK/Texture Wad Files/spraypaint.wad;"
                + home + "/.steam/steam/steamapps/common/Half-Life SDK/Texture Wad Files/xeno.wad"
            );
#endif

            std::stack<RMF::Group> groups{};
            groups.push(map.objects);
            while (!groups.empty())
            {
                auto group = groups.top();
                groups.pop();
                for (auto const &brush : group.brushes)
                    worldspawn.brushes.push_back(brush);
                for (auto const &entity : group.entities)
                    entities.push_back(entity);
                for (auto group2 : group.groups)
                    groups.push(group2);
            }
            entities.emplace_back(worldspawn);
        }

        // TEMP
        operator MAP::Map() const
        {
            MAP::Map out{};
            for (auto const &entity : entities)
                out.entities.push_back(entity);
            return out;
        }

    private:
        // TODO:
        // - visgroups & groups
        // - paths (what are these?)
        // - cameras
    };
}
}

#endif
