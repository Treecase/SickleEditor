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

#include <se-lua/se-lua.hpp>

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <glibmm/property.h>

#include <string>


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
        LuaConsole(LuaConsole &other)=delete;
        ~LuaConsole();

        auto property_lua_state() {return _prop_lua_state.get_proxy();}
        auto property_lua_state() const {return _prop_lua_state.get_proxy();}

        /** Write some text to the console output widget without a newline. */
        void write(std::string const &str);
        /** Write some text to the console output widget with a newline. */
        void writeline(std::string const &str);

        // NOTE: Has to be public due to the implementation. This method gets
        // called by a C function passed to Lua.
        /**
         * Replaces standard Lua print(). Writes to the console output widget
         * instead.
         */
        int _print_override();
        // NOTE: Has to be public due to the implementation. This method gets
        // called by a C function passed to Lua.
        /** Updates internal debug info struct. */
        void _debug_hook(lua_Debug *ar);

    protected:
        /** Called when Lua panics. */
        void error_handler(lua_State *L);
        /** Hook into the Lua state. */
        void do_hooks(lua_State *L);
        /** Unhook from the Lua state. */
        void undo_hooks(lua_State *L);

        void on_input_activated();

    private:
        Glib::Property<gpointer> _prop_lua_state;
        DebugInfo _debug{};
        FILE *_fp{nullptr};

        // Important widgets
        Gtk::TextView _output{};
        Gtk::Entry _input{};
        // Structural widgets
        Gtk::ScrolledWindow _scrollwin{};

        void _on_lua_state_changed();
        /** Shorthand for accessing the "lua_state" property. */
        lua_State *get_L() const;
    };
}

#endif
