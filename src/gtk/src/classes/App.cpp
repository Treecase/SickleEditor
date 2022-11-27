/**
 * App.cpp - Sickle app.
 * Copyright (C) 2022 Trevor Last
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
#include "AppWin.hpp"

#include "appid.hpp"

#include <iostream>


Glib::RefPtr<Sickle::App> Sickle::App::create()
{
    return Glib::RefPtr{new App{}};
}


Sickle::App::App()
:   Gtk::Application(SE_APPLICATION_ID, Gio::ApplicationFlags::APPLICATION_HANDLES_OPEN)
,   game_definition_path{"notes/map/halflife.fgd"}
,   game_definition{FGD::load(game_definition_path)}
{
}

void Sickle::App::on_startup()
{
    // Call the base class's implementation.
    Gtk::Application::on_startup();

    // Add actions for the menu.
    // File
    add_action("new", sigc::mem_fun(*this, &App::on_action_new));
    add_action("open", sigc::mem_fun(*this, &App::on_action_open));
    add_action("exit", sigc::mem_fun(*this, &App::on_action_exit));
    // About
    add_action("about", sigc::mem_fun(*this, &App::on_action_about));
    // Add keyboard accelerators for the menu.
    set_accel_for_action("app.new", "<Ctrl>N");
    set_accel_for_action("app.open", "<Ctrl>O");
    set_accel_for_action("app.exit", "<Ctrl>Q");
}

void Sickle::App::on_activate()
{
    auto appwindow = create_appwindow();
    appwindow->present();
}

void Sickle::App::on_open(Gio::Application::type_vec_files const &files, Glib::ustring const &hint)
{
    // Use already existing AppWin if it exists, otherwise create a new one.
    AppWin *appwindow = nullptr;
    auto windows = get_windows();
    if (windows.size() > 0)
        appwindow = dynamic_cast<AppWin *>(windows[0]);
    if (!appwindow)
        appwindow = create_appwindow();
    for (auto const &file : files)
        appwindow->open(file);
    appwindow->present();
}


Sickle::AppWin *Sickle::App::create_appwindow()
{
    auto appwindow = AppWin::create();
    add_window(*appwindow);
    // Delete the window when it is hidden.
    appwindow->signal_hide().connect(sigc::bind(sigc::mem_fun(*this,
      &App::on_hide_window), appwindow));
    return appwindow;
}


void Sickle::App::on_hide_window(Gtk::Window *window)
{
    delete window;
}

void Sickle::App::on_dialog_response(int response, Gtk::Dialog *dialog)
{
    delete dialog;
}

void Sickle::App::on_action_new()
{
    // TODO
}

void Sickle::App::on_action_open()
{
    auto win = dynamic_cast<AppWin *>(get_active_window());
    auto chooser = Gtk::FileChooserNative::create("Open", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN, "Open", "Cancel");
    chooser->set_transient_for(*win);
    auto all_filter = Gtk::FileFilter::create();
    all_filter->add_pattern("*.*");
    all_filter->set_name("All Files");
    auto map_filter = Gtk::FileFilter::create();
    map_filter->add_pattern("*.map");
    map_filter->set_name("Game Maps");
    chooser->add_filter(map_filter);
    chooser->add_filter(all_filter);
    int response = chooser->run();
    switch (response)
    {
    case Gtk::ResponseType::RESPONSE_ACCEPT:
        win->open(chooser->get_file());
        break;
    }
}

void Sickle::App::on_action_exit()
{
    for (auto &window : get_windows())
        window->hide();
    quit();
}

void Sickle::App::on_action_about()
{
    auto about = Sickle::About::create(*get_active_window());
    // Delete the dialog when it is hidden.
    about->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &App::on_hide_window), about));
    about->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &App::on_dialog_response), about));
    about->run();
}
