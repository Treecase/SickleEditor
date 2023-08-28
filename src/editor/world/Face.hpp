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

#include <glibmm.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <string>
#include <vector>


namespace Sickle::Editor
{
    class Face : public Glib::Object
    {
        sigc::signal<void()> _vertices_changed{};
    public:
        std::vector<glm::vec3> vertices{};
        Glib::Property<std::string> texture;
        Glib::Property<glm::vec3> u, v;
        Glib::Property<glm::vec2> shift;
        Glib::Property<glm::vec2> scale;
        Glib::Property<float> rotation;

        // TODO: replace constructors with static create() functions.
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
}

#endif
