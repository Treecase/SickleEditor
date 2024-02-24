/**
 * WADList.cpp - Sickle "Texture WADs" editor.
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

#include "WADList.hpp"

#include <glibmm/convert.h>
#include <gtkmm/dialog.h>
#include <gtkmm/filechoosernative.h>
#include <gtkmm/treerowreference.h>

#include <set>


Sickle::WADList::Columns::Columns()
{
    add(col_path);
}



Sickle::WADList::WADList()
:   Glib::ObjectBase{typeid(WADList)}
,   Gtk::Frame{"Texture WADs"}
,   _prop_wad_paths{*this, "wad-paths", {}}
,   _paths{Gtk::ListStore::create(Columns{})}
,   _pathview{_paths}
{
    _pathview.append_column("WAD", _columns.col_path);

    _add_path.set_hexpand(true);
    _remove_path.set_hexpand(true);

    _button_box.add(_add_path);
    _button_box.add(_remove_path);

    _content.add(_pathview);
    _content.add(_button_box);
    add(_content);

    property_wad_paths().signal_changed().connect(
        sigc::mem_fun(*this, &WADList::_on_wad_paths_changed));

    _add_path.signal_clicked().connect(
        sigc::mem_fun(*this, &WADList::on_add_path_clicked));
    _remove_path.signal_clicked().connect(
        sigc::mem_fun(*this, &WADList::on_remove_path_clicked));
}


void Sickle::WADList::on_add_path_clicked()
{
    auto const chooser = Gtk::FileChooserNative::create(
        "Add Texture WADs",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    chooser->set_select_multiple(true);

    auto const wad_filter = Gtk::FileFilter::create();
    wad_filter->add_pattern("*.wad");
    wad_filter->set_name("Texture WADs");
    chooser->add_filter(wad_filter);

    auto const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
    {
        auto paths = property_wad_paths().get_value();
        for (auto const &filename : chooser->get_filenames())
            paths.insert(Glib::filename_to_utf8(filename));
        property_wad_paths().set_value(paths);
    }
}


void Sickle::WADList::on_remove_path_clicked()
{
    auto const selected_iter = _pathview.get_selection()->get_selected();
    if (selected_iter)
    {
        auto const path = selected_iter->get_value(_columns.col_path);
        auto paths = property_wad_paths().get_value();
        paths.erase(path);
        property_wad_paths().set_value(paths);
    }
}



void Sickle::WADList::_on_wad_paths_changed()
{
    _paths->clear();
    for (auto const &path : property_wad_paths().get_value())
    {
        auto it = _paths->append();
        it->set_value(_columns.col_path, path);
    }
}
