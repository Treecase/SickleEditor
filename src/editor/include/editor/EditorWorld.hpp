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

#include <convexhull/convexhull.hpp>
#include <map/map.hpp>
#include <rmf/rmf.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sigc++/signal.h>

#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>


namespace Sickle
{
    template<typename T>
    class Property
    {
    public:
        Property()=default;
        Property(T init): _value{init} {}
        template<typename... Ts> Property(Ts... args): _value{args...} {}
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
        sigc::signal<void()> _vertices_changed{};
    public:
        std::vector<glm::vec3> vertices{};
        Property<std::string> texture{};
        Property<glm::vec3> u{0.0f}, v{0.0f};
        Property<glm::vec2> shift{0.0f, 0.0f};
        Property<glm::vec2> scale{1.0f, 1.0f};
        Property<float> rotation{0.0f};

        Face(
            HalfPlane const &plane,
            std::vector<glm::vec3> const &brush_vertices);
        Face(
            MAP::Plane const &plane,
            std::unordered_set<glm::vec3> const &brush_vertices);
        Face(RMF::Face const &face);

        operator MAP::Plane() const;

        auto &signal_vertices_changed() {return _vertices_changed;}

        // Sorted same direction as VERTICES list.
        std::array<glm::vec3, 3> get_plane_points() const;

        void set_vertex(size_t index, glm::vec3 vertex);
    };


    class Brush
    {
    public:
        Property<bool> is_selected{false};
        std::vector<std::shared_ptr<Face>> faces{};

        Brush()=default;
        Brush(std::vector<glm::vec3> const &points);
        Brush(MAP::Brush const &brush);
        Brush(RMF::Solid const &solid);

        operator MAP::Brush() const;

        void transform(glm::mat4 const &matrix);
        void translate(glm::vec3 const &translation);

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
        std::vector<std::shared_ptr<Brush>> brushes{};

        Entity()=default;
        Entity(MAP::Entity const &entity);
        Entity(RMF::Entity const &entity);

        operator MAP::Entity() const;

        // TODO:
        // - visgroup id
        // - color
    };


    class Map
    {
    public:
        std::vector<Entity> entities{};

        Map();
        Map(MAP::Map const &map);
        Map(RMF::RichMap const &map);

        void add_brush(std::shared_ptr<Brush> const &brush);

        operator MAP::Map() const;

    private:
        // TODO:
        // - visgroups & groups
        // - paths (what are these?)
        // - cameras
    };
}
}

#endif
