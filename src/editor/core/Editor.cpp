/**
 * Editor.cpp - Editor::Editor.
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

#include "core/Editor.hpp"

using namespace Sickle::Editor;


EditorRef Editor::create(lua_State *L)
{
    return EditorRef{new Editor{L}};
}


Editor::Editor(lua_State *L)
:   Glib::ObjectBase{typeid(Editor)}
,   Lua::Referenceable{}
,   oploader{std::make_shared<OperationLoader>(L)}
,   _prop_map{*this, "map", {}}
,   _prop_maptool{*this, "maptool", ""}
,   _prop_mode{*this, "mode", {}}
,   _prop_wads{*this, "wads", {}}
{
    property_map().signal_changed().connect(
        sigc::mem_fun(*this, &Editor::_on_map_changed));
    property_wads().signal_changed().connect(
        sigc::mem_fun(*this, &Editor::_on_wads_changed));

    set_map(World::create());
}


MapTool Editor::get_maptool() const
{
    try {
        return _maptools.at(property_maptool().get_value());
    }
    catch (std::out_of_range const &e) {
        return MapTool{"", {}, [](auto){return false;}};
    }
}


void Editor::add_maptool(MapTool const &maptool)
{
    _maptools.insert({maptool.name(), maptool});
    signal_maptools_changed().emit();
    if (property_maptool().get_value().empty())
        set_maptool(maptool.name());
}



void Editor::on_object_selected_changed(EditorObjectRef const &obj)
{
    if (obj->is_selected())
        selected.add(obj);
    else
        selected.remove(obj);
}


void Editor::on_object_added(EditorObjectRef const &obj)
{
    // obj will be automatically added/removed from Selection.
    obj->property_selected().signal_changed().connect(
        sigc::bind(
            sigc::mem_fun(*this, &Editor::on_object_selected_changed),
            obj));

    // Existing children will act the same as obj.
    obj->foreach([this](auto child){on_object_added(child);});

    // New children will act the same as obj.
    obj->signal_child_added().connect(
        sigc::mem_fun(*this, &Editor::on_object_added));
}



void Editor::_on_map_changed()
{
    brushbox.p1(glm::vec3{});
    brushbox.p2(glm::vec3{});
    selected.clear();

    auto world = get_map();
    on_object_added(world);
}


void Editor::_on_wads_changed()
{
    auto &texman = WAD::TextureManager::get_reference();
    for (auto const &path : get_wads())
    {
        auto const wad = WAD::load(path);
        texman.add_wad(wad);
    }
}
