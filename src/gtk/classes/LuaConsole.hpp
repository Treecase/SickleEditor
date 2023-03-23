/**
 * LuaConsole.hpp - Sickle editor Lua console.
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

#ifndef SE_LUACONSOLE_HPP
#define SE_LUACONSOLE_HPP

#include "se-lua/se-lua.hpp"

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <glibmm/property.h>


namespace Sickle
{
    class LuaConsole : public Gtk::Box
    {
    public:
        struct DebugInfo
        {
            int line_number{-1};
            std::string where;
        };

        LuaConsole();

        auto property_lua_state() {return _prop_lua_state.get_proxy();}

        void write(std::string const &str);
        void writeline(std::string const &str);
        int _print_override();
        void _debug_hook(lua_Debug *ar);
    protected:
        Gtk::ScrolledWindow m_scroll;
        Gtk::TextView m_output;
        Gtk::Entry m_input;

        void error_handler(lua_State *L);

        void on_input_activated();
    private:
        Glib::Property<gpointer> _prop_lua_state;
        DebugInfo debug{};
        lua_State *L{nullptr};
        FILE *_fp{nullptr};

        void _on_lua_state_changed();
    };
}

#endif
