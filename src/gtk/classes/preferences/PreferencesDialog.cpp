/**
 * PreferencesDialog.cpp - Sickle Preferences dialog.
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

#include "PreferencesDialog.hpp"
#include "appid.hpp"

#include <glibmm/convert.h>
#include <gtkmm/filechoosernative.h>


using namespace Sickle;


PreferencesDialog::PreferencesDialog(Gtk::Window &parent)
:   Glib::ObjectBase{typeid(PreferencesDialog)}
,   Gtk::Dialog{"Preferences", parent, false}
,   _settings{Gio::Settings::create(SE_APPLICATION_ID)}
{
    _gamedef_entry.set_text(_settings->get_string("fgd-path"));
    _gamedef_entry.property_secondary_icon_name() = "folder";
    _gamedef_entry.property_secondary_icon_activatable() = true;

    _sprite_path_entry.set_text(_settings->get_string("sprite-root-path"));
    _sprite_path_entry.property_secondary_icon_name() = "folder";
    _sprite_path_entry.property_secondary_icon_activatable() = true;

    auto const wad_paths = _settings->get_string_array("wad-paths");
    _wads.property_wad_paths().set_value(
        std::set<Glib::ustring>(wad_paths.begin(), wad_paths.end()));

    _grid.attach(_gamedef_label, 0, 0);
    _grid.attach(_gamedef_entry, 1, 0);

    _grid.attach(_sprite_path_label, 0, 1);
    _grid.attach(_sprite_path_entry, 1, 1);

    _grid.attach(_wads, 0, 2, 2);

    add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
    add_button("Confirm", Gtk::ResponseType::RESPONSE_ACCEPT);

    get_content_area()->add(_grid);

    _gamedef_entry.signal_icon_press().connect(
        sigc::mem_fun(
            *this,
            &PreferencesDialog::on_gamedef_entry_icon_pressed));
    _sprite_path_entry.signal_icon_press().connect(
        sigc::mem_fun(
            *this,
            &PreferencesDialog::on_sprite_path_entry_icon_pressed));

    show_all_children();
}



void PreferencesDialog::on_response(int response_id)
{
    Gtk::Dialog::on_response(response_id);
    if (response_id == Gtk::ResponseType::RESPONSE_ACCEPT)
        _apply_preferences();
    hide();
}



void PreferencesDialog::_apply_preferences()
{
    _settings->set_string("sprite-root-path", _sprite_path_entry.get_text());
    _settings->set_string("fgd-path", _gamedef_entry.get_text());
    _settings->set_string_array(
        "wad-paths",
        _wads.property_wad_paths().get_value());
}


void PreferencesDialog::on_gamedef_entry_icon_pressed(
    Gtk::EntryIconPosition icon_pos,
    GdkEventButton const *event)
{
    auto const chooser = Gtk::FileChooserNative::create(
        "Select Game Definition",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    chooser->set_transient_for(*this);
    chooser->set_filename(Glib::filename_from_utf8(_gamedef_entry.get_text()));

    auto const all_filter = Gtk::FileFilter::create();
    all_filter->add_pattern("*.*");
    all_filter->set_name("All Files");
    chooser->add_filter(all_filter);

    auto const map_filter = Gtk::FileFilter::create();
    map_filter->add_pattern("*.fgd");
    map_filter->set_name("Game Data Files");
    chooser->add_filter(map_filter);

    int const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
    {
        _gamedef_entry.set_text(
            Glib::filename_to_utf8(chooser->get_filename()));
    }
}


void PreferencesDialog::on_sprite_path_entry_icon_pressed(
    Gtk::EntryIconPosition icon_pos,
    GdkEventButton const *event)
{
    auto const chooser = Gtk::FileChooserNative::create(
        "Select Sprite Root Directory",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    chooser->set_transient_for(*this);
    chooser->set_filename(
        Glib::filename_from_utf8(_sprite_path_entry.get_text()));

    int const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
    {
        _sprite_path_entry.set_text(
            Glib::filename_to_utf8(chooser->get_filename()));
    }
}
