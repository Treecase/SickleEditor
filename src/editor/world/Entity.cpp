/**
 * Entity.cpp - Editor::Entity.
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

#include "world/Entity.hpp"

#include <algorithm>

using namespace Sickle::Editor;



Entity::Entity()
:   Glib::ObjectBase{typeid(Entity)}
{
}


Entity::Entity(Entity const &other)
:   Entity{}
{
    properties = other.properties;
    _brushes = other._brushes;
}


Entity::Entity(MAP::Entity const &entity)
:   Entity{}
{
    properties = entity.properties;
    for (auto const &brush : entity.brushes)
        _brushes.push_back(Glib::RefPtr{new Brush{brush}});
}


Entity::Entity(RMF::Entity const &entity)
:   Entity{}
{
    properties = entity.kv_pairs;
    properties["classname"] = entity.classname;
    for (auto const &brush : entity.brushes)
        _brushes.push_back(Glib::RefPtr{new Brush{brush}});
}


Entity::operator MAP::Entity() const
{
    MAP::Entity out{};
    out.properties = properties;
    for (auto const &brush : _brushes)
        out.brushes.push_back(*brush.get());
    return out;
}


Entity &Entity::operator=(Entity const &other)
{
    if (this != &other)
    {
        properties = other.properties;
        _brushes = other._brushes;
    }
    return *this;
}


std::vector<BrushRef> Entity::brushes() const
{
    std::vector<BrushRef> the_brushes{};
    for (auto const brush : _brushes)
        the_brushes.emplace_back(brush);
    return the_brushes;
}


void Entity::add_brush(Brush const &brush)
{
    Glib::RefPtr the_brush{new Brush{brush}};
    _brushes.push_back(the_brush);
    the_brush->property_real() = true;
    signal_changed().emit();
}


void Entity::remove_brush(BrushRef const &brush)
{
    auto const it = std::find(_brushes.begin(), _brushes.end(), brush);
    if (it == _brushes.end())
        return;
    (*it)->property_real() = false;
    _brushes.erase(it);
    signal_changed().emit();
}
