/**
 * Entity.cpp - Editor::Entity.
 * Copyright (C) 2023-2024 Trevor Last
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

#include "Entity.hpp"

#include <config/appid.hpp>

#include <algorithm>
#include <iostream> // temp

using namespace Sickle::Editor;


EntityRef Entity::create()
{
    return EntityRef{new Entity{}};
}


EntityRef Entity::create(MAP::Entity const &entity)
{
    auto e = create();
    for (auto const &kv : entity.properties)
        e->set_property(kv.first, kv.second);
    for (auto const &brush : entity.brushes)
        e->add_brush(Brush::create(brush));
    return e;
}


EntityRef Entity::create(RMF::Entity const &entity)
{
    auto e = create();
    for (auto const &kv : entity.kv_pairs)
        e->set_property(kv.first, kv.second);

    e->set_property("classname", entity.classname);

    if (e->classinfo().type == "PointClass")
    {
        std::stringstream origin_str{};
        origin_str << entity.position.x << ' ' << entity.position.y << ' ' << entity.position.z;
        e->set_property("origin", origin_str.str());
    }

    for (auto const &brush : entity.brushes)
        e->add_brush(Brush::create(brush));
    return e;
}


Entity::Entity()
:   Glib::ObjectBase{typeid(Entity)}
{
    _properties["classname"] = "";
    _classinfo.type = "<undefined>";
}


Entity::~Entity()
{
    for (auto const &child : brushes())
        remove_brush(child);
}


Entity::operator MAP::Entity() const
{
    MAP::Entity out{};
    out.properties = _properties;
    for (auto const &brush : _brushes)
        out.brushes.push_back(*brush.get());
    return out;
}


EntityClass Entity::classinfo() const
{
    return _classinfo;
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
    if (key == "classname")
        _on_classname_changed();
    signal_properties_changed().emit();
}


bool Entity::remove_property(std::string const &key)
{
    if (key == "classname")
        return false;
    bool const v = _properties.erase(key);
    if (v)
        signal_properties_changed().emit();
    return v;
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


std::vector<EditorObjectRef> Entity::children() const
{
    std::vector<EditorObjectRef> out{};
    for (auto const &brush : brushes())
        out.push_back(brush);
    return out;
}



void Entity::_on_classname_changed()
{
    auto &games = GameDefinition::instance();
    try {
        _classinfo = games.lookup(classname());
    }
    catch (std::out_of_range const &) {
        std::cout << "failed to find class '" << classname() << "'\n";
        _classinfo.type = "<undefined>";
        _classinfo.properties.clear();
        _classinfo.entity_properties.clear();
    }

    for (auto const &property : _classinfo.entity_properties)
        _properties.insert({property.first, ""});

    if (_classinfo.type == "PointClass")
        _properties.insert({"origin", "0 0 0"});
}
