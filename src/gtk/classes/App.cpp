/**
 * App.cpp - Sickle app.
 * Copyright (C) 2022-2023 Trevor Last
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

#include "App.hpp"
#include "About.hpp"
#include "appid.hpp"
#include "WADDialog.hpp"

#include <gtkmm/filechoosernative.h>


Glib::RefPtr<Sickle::App> Sickle::App::create()
{
    return Glib::RefPtr{new App{}};
}



Sickle::App::App()
:   Glib::ObjectBase{typeid(App)}
,   Gtk::Application{
        SE_APPLICATION_ID, Gio::ApplicationFlags::APPLICATION_HANDLES_OPEN}
,   _settings{Gio::Settings::create(SE_APPLICATION_ID)}
,   _prop_fgd_path{*this, "fgd-path", ""}
,   _prop_wad_paths{*this, "wad-paths", {}}
{
    property_fgd_path().signal_changed().connect(
        sigc::mem_fun(*this, &App::_on_fgd_path_changed));
    _settings->bind("fgd-path", property_fgd_path());
    _settings->bind("wad-paths", property_wad_paths());
}


void Sickle::App::on_startup()
{
    Gtk::Application::on_startup();

    // Add actions for the menu.
    // File
    add_action("new", sigc::mem_fun(*this, &App::on_action_new));
    add_action("open", sigc::mem_fun(*this, &App::on_action_open));
    add_action("save", sigc::mem_fun(*this, &App::on_action_save));
    add_action("exit", sigc::mem_fun(*this, &App::on_action_exit));
    // Edit
    add_action("setGameDef", sigc::mem_fun(*this, &App::on_action_setGameDef));
    add_action(
        "setWADPaths",
        sigc::mem_fun(*this, &App::on_action_setWADPaths));
    // About
    add_action("about", sigc::mem_fun(*this, &App::on_action_about));

    // Add keyboard accelerators for the menu.
    set_accel_for_action("app.new", "<Ctrl>N");
    set_accel_for_action("app.open", "<Ctrl>O");
    set_accel_for_action("app.save", "<Ctrl>S");
    set_accel_for_action("app.exit", "<Ctrl>Q");
    set_accel_for_action("win.openLuaConsole", "<Ctrl><Shift>C");
    set_accel_for_action("win.reloadLua", "<Ctrl><Shift>R");

    // Add keyboard accelerators for MapTools.
    set_accel_for_action("win.mapTools_Select", "<Shift>S");
    set_accel_for_action("win.mapTools_CreateBrush", "<Shift>B");
}


void Sickle::App::on_activate()
{
    auto appwindow = _create_appwindow();
    appwindow->maximize();
    appwindow->present();
}


void Sickle::App::on_open(
    Gio::Application::type_vec_files const &files, Glib::ustring const &hint)
{
    // Use already existing AppWin if it exists, otherwise create a new one.
    AppWin::AppWin *appwindow = nullptr;
    auto const windows = get_windows();
    if (windows.size() > 0)
        appwindow = dynamic_cast<AppWin::AppWin *>(windows.at(0));
    if (!appwindow)
        appwindow = _create_appwindow();
    appwindow->open(files.at(0));
    appwindow->maximize();
    appwindow->present();
}


void Sickle::App::on_action_new()
{
    auto win = dynamic_cast<AppWin::AppWin *>(get_active_window());
    win->open(Glib::RefPtr<Gio::File>{nullptr});
}


void Sickle::App::on_action_open()
{
    auto win = dynamic_cast<AppWin::AppWin *>(get_active_window());
    auto chooser = Gtk::FileChooserNative::create(
        "Open",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    chooser->set_transient_for(*win);

    auto all_filter = Gtk::FileFilter::create();
    all_filter->add_pattern("*.*");
    all_filter->set_name("All Files");
    chooser->add_filter(all_filter);

    auto map_filter = Gtk::FileFilter::create();
    map_filter->add_pattern("*.map");
    map_filter->set_name("Game Maps");
    chooser->add_filter(map_filter);

    auto rmf_filter = Gtk::FileFilter::create();
    rmf_filter->add_pattern("*.rmf");
    rmf_filter->set_name("Hammer/Worldcraft Maps");
    chooser->add_filter(rmf_filter);
    chooser->set_filter(rmf_filter);

    int const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
        win->open(chooser->get_file());
}


void Sickle::App::on_action_save()
{
    auto win = dynamic_cast<AppWin::AppWin *>(get_active_window());
    auto chooser = Gtk::FileChooserNative::create(
        "Save",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SAVE);
    chooser->set_transient_for(*win);

    auto all_filter = Gtk::FileFilter::create();
    all_filter->add_pattern("*.*");
    all_filter->set_name("All Files");
    chooser->add_filter(all_filter);

    auto map_filter = Gtk::FileFilter::create();
    map_filter->add_pattern("*.map");
    map_filter->set_name("Game Maps");
    chooser->add_filter(map_filter);
    chooser->set_filter(map_filter);

    int const response = chooser->run();

    auto filename = chooser->get_filename();
    if (chooser->get_filter() == map_filter)
    {
        auto const x = filename.rfind(".map");
        if (x == std::string::npos)
            filename.append(".map");
    }

    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
        win->save(filename);
}


void Sickle::App::on_action_exit()
{
    for (auto &window : get_windows())
        window->hide();
    quit();
}


void Sickle::App::on_action_setGameDef()
{
    auto chooser = Gtk::FileChooserNative::create(
        "Open",
        Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    chooser->set_transient_for(*get_active_window());

    auto all_filter = Gtk::FileFilter::create();
    all_filter->add_pattern("*.*");
    all_filter->set_name("All Files");
    chooser->add_filter(all_filter);

    auto map_filter = Gtk::FileFilter::create();
    map_filter->add_pattern("*.fgd");
    map_filter->set_name("Game Data Files");
    chooser->add_filter(map_filter);

    int const response = chooser->run();
    if (response == Gtk::ResponseType::RESPONSE_ACCEPT)
        property_fgd_path().set_value(chooser->get_filename());
}


void Sickle::App::on_action_setWADPaths()
{
    auto win = dynamic_cast<AppWin::AppWin *>(get_active_window());
    auto waddialog = WADDialog{*win};
    waddialog.set_transient_for(*win);
    int const response = waddialog.run();
    _sync_wadpaths(win);
}


void Sickle::App::on_action_about()
{
    auto about = Sickle::About{};
    about.set_transient_for(*get_active_window());
    about.run();
}



Sickle::AppWin::AppWin *Sickle::App::_create_appwindow()
{
    auto appwindow = new AppWin::AppWin{};
    _sync_wadpaths(appwindow);
    add_window(*appwindow);
    // Delete the window when it is hidden.
    appwindow->signal_hide().connect(
        sigc::bind(sigc::mem_fun(*this, &App::_on_hide_window), appwindow));
    return appwindow;
}


void Sickle::App::_sync_wadpaths(AppWin::AppWin *appwin)
{
    auto const settings = Gio::Settings::create(SE_APPLICATION_ID);
    appwin->editor.wads.set(settings->get_string_array("wad-paths"));
}


void Sickle::App::_on_hide_window(Gtk::Window *window)
{
    delete window;
}


void Sickle::App::_on_fgd_path_changed()
{
    auto const path = property_fgd_path().get_value();
    if (!path.empty())
        _game_definition = FGD::from_file(property_fgd_path().get_value());
}
