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

#include <appid.hpp>

#include <algorithm>

using namespace Sickle::Editor;


EntityRef Entity::create()
{
    return EntityRef{new Entity{}};
}


EntityRef Entity::create(MAP::Entity const &entity)
{
    auto e = create();
    e->_properties = entity.properties;
    for (auto const &brush : entity.brushes)
        e->add_brush(Brush::create(brush));
    return e;
}


EntityRef Entity::create(RMF::Entity const &entity)
{
    auto e = create();
    e->_properties = entity.kv_pairs;
    e->_properties["classname"] = entity.classname;
    for (auto const &brush : entity.brushes)
        e->add_brush(Brush::create(brush));
    return e;
}


Entity::Entity()
:   Glib::ObjectBase{typeid(Entity)}
{
    _properties["classname"] = "";
}


Entity::operator MAP::Entity() const
{
    MAP::Entity out{};
    out.properties = _properties;
    for (auto const &brush : _brushes)
        out.brushes.push_back(*brush.get());
    return out;
}


std::string Entity::classname() const
{
    return _properties.at("classname");
}


std::unordered_map<std::string, std::string> Entity::properties() const
{
    return _properties;
}


std::string Entity::get_property(std::string const &key) const
{
    return _properties.at(key);
}


void Entity::set_property(std::string const &key, std::string const &value)
{
    _properties[key] = value;
}


std::vector<BrushRef> Entity::brushes() const
{
    return _brushes;
}


void Entity::add_brush(BrushRef const &brush)
{
    _brushes.push_back(brush);
    signal_child_added().emit(brush);
}


void Entity::remove_brush(BrushRef const &brush)
{
    auto const it = std::find(_brushes.begin(), _brushes.end(), brush);
    if (it == _brushes.end())
        return;
    _brushes.erase(it);
    signal_child_removed().emit(brush);
}


/* ---[ EditorObject interface ]--- */
Glib::ustring Entity::name() const
{
    std::stringstream ss{};
    ss << _properties.at("classname") << ' ' << this;
    return Glib::ustring{ss.str()};
}


Glib::RefPtr<Gdk::Pixbuf> Entity::icon() const
{
    return Gdk::Pixbuf::create_from_resource(
        SE_GRESOURCE_PREFIX "icons/outliner/entity.png");
}


std::vector<Glib::RefPtr<EditorObject>> Entity::children() const
{
    std::vector<Glib::RefPtr<EditorObject>> out{};
    for (auto const &brush : brushes())
        out.push_back(brush);
    return out;
}
