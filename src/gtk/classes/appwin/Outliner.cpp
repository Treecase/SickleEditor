/**
 * Outliner.cpp - The Sickle object outliner.
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

#include "Outliner.hpp"
#include <appid.hpp>

#include <sstream>

using namespace Sickle::AppWin;


Outliner::Outliner()
:   Glib::ObjectBase{typeid(Outliner)}
,   Gtk::Bin{}
,   _prop_world{*this, "world", Glib::RefPtr<Editor::World>{nullptr}}
{
    _tree_view.set_model(_tree_store);
    _tree_view.append_column("Icon", _tree_columns.icon);
    _tree_view.append_column("Label", _tree_columns.text);
    _tree_view.set_headers_visible(false);

    _scrolled.add(_tree_view);

    add(_scrolled);

    property_world().signal_changed().connect(
        sigc::mem_fun(*this, &Outliner::on_world_changed));
}


void Outliner::on_world_changed()
{
    _tree_store->clear();

    auto const &w = property_world().get_value();
    for (auto const &e : w->entities())
    {
        auto &ent_row = *_tree_store->append();
        ent_row[_tree_columns.text] = e->name();
        ent_row[_tree_columns.icon] = e->icon();

        for (auto const &child : e->children())
            _add_object(child, ent_row);
    }
}


void Outliner::_add_object(
    Sickle::Editor::EditorObject *obj,
    Gtk::TreeRow const &parent_row)
{
    auto &row = *_tree_store->append(parent_row.children());
    row[_tree_columns.text] = obj->name();
    row[_tree_columns.icon] = obj->icon();

    for (auto const &child : obj->children())
        _add_object(child, row);
}
