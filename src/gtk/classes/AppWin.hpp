/**
 * AppWin.hpp - Sickle ApplicationWindow.
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

#ifndef SE_APPWIN_HPP
#define SE_APPWIN_HPP

#include "MapArea.hpp"
#include "MapArea2D.hpp"
#include "LuaConsole.hpp"
#include "editor/Editor.hpp"
#include "se-lua/se-lua.hpp"

#include <glibmm/property.h>
#include <glibmm/binding.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/label.h>


namespace Sickle
{
    class AppWin : public Gtk::ApplicationWindow
    {
    public:
        Editor editor;
        lua_State *L;

        AppWin();

        /** Open a file. */
        void open(Gio::File const *file);
        void show_console_window();

        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        void set_grid_size(guint grid_size);
        guint get_grid_size();

        // Lua constructor needs access to private members.
        friend int lappwin_new(lua_State *, AppWin *);

    protected:
        Gtk::Grid m_grid;
        Gtk::Grid m_viewgrid;
        MapArea m_maparea;
        MapArea2D m_drawarea_top, m_drawarea_front, m_drawarea_right;
        Gtk::HBox m_hbox;
        Gtk::Label m_gridsizelabel;
        Gtk::Window m_luaconsolewindow;
        LuaConsole m_luaconsole;

    private:
        MAP::Map _map;
        Glib::Property<guint> _prop_grid_size;
        Glib::RefPtr<Glib::Binding> _binding_grid_size_top,
            _binding_grid_size_front,
            _binding_grid_size_right;

        void _on_grid_size_changed();
    };
}

#endif
