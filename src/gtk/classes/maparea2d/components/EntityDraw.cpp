/**
 * EntityDraw.cpp - Cairo Editor::Entity view.
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

#include "EntityDraw.hpp"

#include <sstream>


using namespace World2D;


/**
 * Extract a vector from a string.
 *
 * @param input Input stream to read from.
 * @param output Vector to write the value to. Not written to if reading fails.
 * @return True if the vector was read, false if an error occurred.
 */
static bool extract_vector(std::stringstream &input, glm::vec3 &output);


static constexpr float DEFAULT_BOX_SIZE = 32.0f;


void EntityDraw::draw(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Sickle::MapArea2D const &maparea) const
{
    if (!cr || !_entity)
        return;

    std::stringstream origin_str{_entity->get_property("origin")};
    glm::vec3 origin3{0.0f, 0.0f, 0.0f};
    extract_vector(origin_str, origin3);
    glm::vec2 const origin = maparea.worldspace_to_drawspace(origin3);

    auto A = origin + DEFAULT_BOX_SIZE * glm::vec2{-0.5f, -0.5f};
    auto B = origin + DEFAULT_BOX_SIZE * glm::vec2{+0.5f, +0.5f};

    auto const classinfo = _entity->classinfo();
    if (auto const size_prop = classinfo
        .get_class_property<Sickle::Editor::ClassPropertySize>())
    {
        auto const points = size_prop->get_points();
        A = origin + maparea.worldspace_to_drawspace(points.first);
        B = origin + maparea.worldspace_to_drawspace(points.second);
    }


    auto const style = maparea.get_style_context();
    style->context_save();
    style->add_class("entity");

    if (_entity->is_selected())
    {
        style->set_state(
            style->get_state() | Gtk::StateFlags::STATE_FLAG_SELECTED);
    }

    auto color = style->get_color(style->get_state());
    if (!_entity->is_selected())
    {
        if (auto const color_prop = classinfo
            .get_class_property<Sickle::Editor::ClassPropertyColor>())
        {
            auto const rgb = color_prop->get_color();
            color.set_red(rgb.r);
            color.set_green(rgb.g);
            color.set_blue(rgb.b);
        }
    }
    cr->set_source_rgb(
        color.get_red(),
        color.get_green(),
        color.get_blue());


    auto const topleft = glm::min(A, B);
    auto const dimensions = glm::abs(A - B);
    cr->rectangle(topleft.x, topleft.y, dimensions.x, dimensions.y);


    cr->stroke();
    style->context_restore();
}



void EntityDraw::on_attach(Sickle::Componentable &obj)
{
    if (_entity)
        throw std::logic_error{"already attached"};

    _entity = &dynamic_cast<Sickle::Editor::Entity const &>(obj);
    if (_entity->classinfo().type() != "PointClass")
        throw std::invalid_argument{"must be PointClass"};
}


void EntityDraw::on_detach(Sickle::Componentable &obj)
{
    _entity = nullptr;
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
