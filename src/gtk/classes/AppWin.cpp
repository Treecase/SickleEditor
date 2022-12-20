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

#include "appid.hpp"
#include "version.hpp"


Sickle::AppWin::AppWin()
:   Gtk::ApplicationWindow{}
,   m_grid{}
,   m_maparea{}
,   m_drawarea_top{}
,   m_drawarea_front{}
,   m_drawarea_right{}
{
    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);

    m_grid.set_row_spacing(2);
    m_grid.set_column_spacing(2);
    m_grid.set_row_homogeneous(true);
    m_grid.set_column_homogeneous(true);

    m_drawarea_top.property_name() = "top";
    m_drawarea_front.property_name() = "front";
    m_drawarea_right.property_name() = "right";

    m_grid.attach(m_maparea, 0, 0);
    m_grid.attach(m_drawarea_top, 1, 0);
    m_grid.attach(m_drawarea_front, 0, 1);
    m_grid.attach(m_drawarea_right, 1, 1);
    add(m_grid);

    show_all_children();
}

void Sickle::AppWin::open(Gio::File const *file)
{
    if (file)
    {
        auto const map = MAP::load(file->get_path());
        m_maparea.set_map(&map);
        m_drawarea_top.set_map(&map);
        m_drawarea_front.set_map(&map);
        m_drawarea_right.set_map(&map);
    }
    else
    {
        m_maparea.set_map(nullptr);
        m_drawarea_top.set_map(nullptr);
        m_drawarea_front.set_map(nullptr);
        m_drawarea_right.set_map(nullptr);
    }
}
