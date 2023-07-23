/**
 * WADDialog.cpp - Sickle "Texture WADs" dialog.
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

#include "WADDialog.hpp"
#include "appid.hpp"

#include <glibmm/convert.h>
#include <gtkmm/filechoosernative.h>
#include <gtkmm/treerowreference.h>

#include <set>


Sickle::WADModelColumns::WADModelColumns()
{
    add(col_path);
}


Sickle::WADDialog::WADDialog(Gtk::Window &parent)
:   Glib::ObjectBase{typeid(WADDialog)}
,   Gtk::Dialog{"Texture WADs", parent, true}
,   _settings{Gio::Settings::create(SE_APPLICATION_ID)}
,   _paths{Gtk::ListStore::create(WADModelColumns{})}
{
    _add_path.signal_clicked().connect(
        sigc::mem_fun(*this, &WADDialog::on_add_path_clicked));
    _remove_path.signal_clicked().connect(
        sigc::mem_fun(*this, &WADDialog::on_remove_path_clicked));
    _settings->signal_changed("wad-paths").connect_notify(
        sigc::mem_fun(*this, &WADDialog::_on_paths_updated));
    _on_paths_updated("wad-paths");

    add_button("Done", Gtk::ResponseType::RESPONSE_ACCEPT);

    _pathview.set_model(_paths);
    _pathview.append_column("WAD", _columns.col_path);

    _add_path.set_hexpand(true);
    _remove_path.set_hexpand(true);
    _button_box.add(_add_path);
    _button_box.add(_remove_path);

    get_content_area()->add(_pathview);
    get_content_area()->add(_button_box);
    show_all_children();
}


void Sickle::WADDialog::on_response(int response)
{
    close();
}


void Sickle::WADDialog::on_add_path_clicked()
{
    auto chooser = Gtk::FileChooserNative::create(
        "Add Texture WADs",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    chooser->set_transient_for(*this);
    chooser->set_select_multiple(true);

    auto wad_filter = Gtk::FileFilter::create();
    wad_filter->add_pattern("*.wad");
    wad_filter->set_name("Texture WADs");
    chooser->add_filter(wad_filter);

    auto const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
    {
        auto const path_array = _settings->get_string_array("wad-paths");
        std::set<Glib::ustring> path_set{path_array.begin(), path_array.end()};
        for (auto const &filename : chooser->get_filenames())
            path_set.insert(Glib::filename_to_utf8(filename));
        _settings->set_string_array(
            "wad-paths",
            std::vector<Glib::ustring>{path_set.begin(), path_set.end()});
    }
}


void Sickle::WADDialog::on_remove_path_clicked()
{
    auto const selected_iter = _pathview.get_selection()->get_selected();
    if (selected_iter)
    {
        auto const path = selected_iter->get_value(_columns.col_path);
        auto const path_array = _settings->get_string_array("wad-paths");
        std::set<Glib::ustring> path_set{path_array.begin(), path_array.end()};
        path_set.erase(path);
        _settings->set_string_array(
            "wad-paths",
            std::vector<Glib::ustring>{path_set.begin(), path_set.end()});
    }
}


void Sickle::WADDialog::_on_paths_updated(Glib::ustring)
{
    _paths->clear();
    auto path_array = _settings->get_string_array("wad-paths");
    for (auto const &path : path_array)
    {
        auto row = *_paths->append();
        row[_columns.col_path] = path;
    }
}
