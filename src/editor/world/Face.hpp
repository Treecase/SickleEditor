/**
 * Face.hpp - Editor::Face.
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

#ifndef SE_EDITOR_WORLD_FACE_HPP
#define SE_EDITOR_WORLD_FACE_HPP

#include <convexhull/convexhull.hpp>
#include <map/map.hpp>
#include <rmf/rmf.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/object.h>
#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>


namespace Sickle::Editor
{
    class Face;
    using FaceRef = Glib::RefPtr<Face>;

    class Face : public Glib::Object, public Lua::Referenceable
    {
    public:
        static FaceRef create(
            HalfPlane const &plane,
            std::vector<glm::vec3> const &brush_vertices);
        static FaceRef create(
            MAP::Plane const &plane,
            std::unordered_set<glm::vec3> const &brush_vertices);
        static FaceRef create(RMF::Face const &face);

        operator MAP::Plane() const;

        auto property_texture() {return _prop_texture.get_proxy();};
        auto property_texture() const {return _prop_texture.get_proxy();};
        auto property_u() {return _prop_u.get_proxy();};
        auto property_u() const {return _prop_u.get_proxy();};
        auto property_v() {return _prop_v.get_proxy();};
        auto property_v() const {return _prop_v.get_proxy();};
        auto property_shift() {return _prop_shift.get_proxy();};
        auto property_shift() const {return _prop_shift.get_proxy();};
        auto property_scale() {return _prop_scale.get_proxy();};
        auto property_scale() const {return _prop_scale.get_proxy();};
        auto property_rotation() {return _prop_rotation.get_proxy();};
        auto property_rotation() const {return _prop_rotation.get_proxy();};

        auto get_texture() const {return _prop_texture.get_value();};
        auto get_u() const {return _prop_u.get_value();};
        auto get_v() const {return _prop_v.get_value();};
        auto get_shift() const {return _prop_shift.get_value();};
        auto get_scale() const {return _prop_scale.get_value();};
        auto get_rotation() const {return _prop_rotation.get_value();};

        void set_texture(std::string const &value) {
            return _prop_texture.set_value(value);};
        void set_u(glm::vec3 const &value) {
            return _prop_u.set_value(value);};
        void set_v(glm::vec3 const &value) {
            return _prop_v.set_value(value);};
        void set_shift(glm::vec2 const &value) {
            return _prop_shift.set_value(value);};
        void set_scale(glm::vec2 const &value) {
            return _prop_scale.set_value(value);};
        void set_rotation(float value) {
            return _prop_rotation.set_value(value);};

        auto &signal_vertices_changed() {return _vertices_changed;}

        /** List of Face vertices. Sorted counterclockwise. */
        auto get_vertices() const {return _vertices;}
        /**
         * 3 points which define the plane of the Face. Sorted counterclockwise.
         */
        std::array<glm::vec3, 3> get_plane_points() const;

        void set_vertex(size_t index, glm::vec3 vertex);
        glm::vec3 get_vertex(size_t index) const;

    private:
        Glib::Property<std::string> _prop_texture;
        Glib::Property<glm::vec3> _prop_u, _prop_v;
        Glib::Property<glm::vec2> _prop_shift;
        Glib::Property<glm::vec2> _prop_scale;
        Glib::Property<float> _prop_rotation;

        sigc::signal<void()> _vertices_changed{};

        std::vector<glm::vec3> _vertices{};

        Face();
    };
}

#endif
