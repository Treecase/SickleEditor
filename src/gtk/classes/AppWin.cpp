/**
 * AppWin.cpp - Sickle ApplicationWindow.
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

#include "AppWin.hpp"

#include <iostream>

#include "appid.hpp"
#include "version.hpp"


Sickle::AppWin::AppWin()
:   Gtk::ApplicationWindow{}
,   m_maparea{}
{
    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);

    add(m_maparea);
    show_all_children();
}

void Sickle::AppWin::open(Gio::File const *file)
{
    if (file)
    {
        auto const map = MAP::load(file->get_path());
        m_maparea.set_map(&map);
    }
    else
        m_maparea.set_map(nullptr);
}


bool Sickle::AppWin::on_key_press_event(GdkEventKey *event)
{
    if (Gtk::Window::on_key_press_event(event))
        return true;
    return m_maparea.on_key_press_event(event);
}

bool Sickle::AppWin::on_key_release_event(GdkEventKey *event)
{
    if (Gtk::Window::on_key_release_event(event))
        return true;
    return m_maparea.on_key_release_event(event);
}
