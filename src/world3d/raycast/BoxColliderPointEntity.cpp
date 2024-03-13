/**
 * BoxColliderPointEntity.cpp - 3D box collider for raycast operations,
 *                              PointEntity tracking version.
 * Copyright (C) 2024 Trevor Last
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

#include "BoxColliderPointEntity.hpp"

#include <sstream>
#include <stdexcept>


using namespace World3D;


/**
 * Extract a vector from a string.
 *
 * @param input Input stream to read from.
 * @param output Vector to write the value to. Not written to if reading fails.
 * @return True if the vector was read, false if an error occurred.
 */
static bool extract_vector(std::stringstream &input, glm::vec3 &output);


void BoxColliderPointEntity::on_attach(Sickle::Componentable &obj)
{
    if (_src)
        throw std::logic_error{"already attached"};
    auto &entity = dynamic_cast<Sickle::Editor::Entity &>(obj);
    auto const entity_class = entity.classinfo();
    if (entity_class.type() != "PointClass")
        throw std::invalid_argument{"must be PointClass"};
    _src = &entity;

    _conn_src_properties_changed = _src->signal_properties_changed().connect(
        sigc::mem_fun(*this, &BoxColliderPointEntity::update_bbox));

    update_bbox();
}


void BoxColliderPointEntity::on_detach(Sickle::Componentable &obj)
{
    _conn_src_properties_changed.disconnect();
    _src = nullptr;
    set_box(BBox3{});
}



void BoxColliderPointEntity::update_bbox()
{
    // Get origin property, bailing if its invalid or doesn't exist.
    std::stringstream origin_str{};
    try {
        origin_str << _src->get_property("origin");
    }
    catch (std::out_of_range const &e) {
        return;
    }
    glm::vec3 origin{};
    if (!extract_vector(origin_str, origin))
        return;

    auto point1 = DEFAULT_SIZE * glm::vec3{-0.5f, -0.5f, -0.5f};
    auto point2 = DEFAULT_SIZE * glm::vec3{+0.5f, +0.5f, +0.5f};

    auto const classinfo = _src->classinfo();
    auto const size =\
        classinfo.get_class_property<Sickle::Editor::ClassPropertySize>();
    if (size)
    {
        auto const points = size->get_points();
        point1 = points.first;
        point2 = points.second;
    }

    BBox3 const bbox{origin + point1, origin + point2};
    set_box(bbox);
}



static bool extract_vector(std::stringstream &input, glm::vec3 &output)
{
    glm::vec3 v{};
    input >> v.x >> v.y >> v.z;
    if (input.fail())
        return false;
    else
    {
        output = v;
        return true;
    }
}
