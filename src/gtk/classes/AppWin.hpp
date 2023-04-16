/**
 * AppWin.hpp - Sickle ApplicationWindow.
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

#ifndef SE_APPWIN_HPP
#define SE_APPWIN_HPP

#include "AppWin_Lua.hpp"
#include "LuaConsole.hpp"
#include "MapArea2D.hpp"
#include "MapArea3D.hpp"
#include "editor/Editor.hpp"

#include <glibmm/property.h>
#include <glibmm/binding.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/infobar.h>
#include <gtkmm/label.h>


namespace Sickle
{
    class AppWin : public Gtk::ApplicationWindow
    {
    public:
        Editor editor;
        lua_State *const L;

        AppWin();

        /** Open a file. */
        void open(Glib::RefPtr<Gio::File> const &file);
        /** Save currently edited map. */
        void save(std::string const &filename);
        /** Open the Lua console window. */
        void show_console_window();
        /** Reload Lua scripts. */
        void reload_scripts();

        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        void set_grid_size(guint grid_size);
        guint get_grid_size();

        auto signal_lua_reloaded() {return _sig_lua_reloaded;}

        // Lua constructor needs access to private members.
        friend void Lua::push(lua_State *, AppWin *);

    protected:
        Gtk::Grid m_grid;
        Gtk::Grid m_viewgrid;
        MapArea3D m_maparea;
        MapArea2D m_drawarea_top, m_drawarea_front, m_drawarea_right;
        Gtk::HBox m_hbox;
        Gtk::Label m_gridsizelabel;
        Gtk::Window m_luaconsolewindow;
        LuaConsole m_luaconsole;
        Gtk::InfoBar m_infobar{};
        Gtk::Label m_infobar_label{};

        void setup_lua_state();

    private:
        MAP::Map _map;
        Glib::Property<guint> _prop_grid_size;
        Glib::RefPtr<Glib::Binding> _binding_grid_size_top,
            _binding_grid_size_front,
            _binding_grid_size_right;
        sigc::signal<void()> _sig_lua_reloaded{};

        std::vector<std::string> _lua_script_dirs{
            SE_DATA_DIR "lua-runtime",
            "../share/lua-runtime",
        };
        std::vector<std::string> const _internal_scripts{
            "lua/gdkevents.lua",
            "lua/gdkkeysyms.lua",
            "lua/gdktypes.lua",
        };

        void _on_grid_size_changed();
    };
}

#endif
