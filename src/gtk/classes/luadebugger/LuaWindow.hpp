/**
 * LuaWindow.hpp - Window containing Lua debugging functionality.
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

#ifndef SE_APPWIN_LUAWINDOW_HPP
#define SE_APPWIN_LUAWINDOW_HPP

#include "LuaDebugger.hpp"

#include <glibmm/binding.h>
#include <gtkmm/box.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/window.h>

namespace Sickle
{
    class LuaWindow : public Gtk::Window
    {
    public:
        LuaWindow();

        auto property_lua_state() { return _prop_lua_state.get_proxy(); }

        auto property_lua_state() const { return _prop_lua_state.get_proxy(); }

        void set_lua_state(lua_State *L) { property_lua_state().set_value(L); }

        lua_State *get_lua_state() const
        {
            return static_cast<lua_State *>(property_lua_state().get_value());
        }

        /** Whether the Lua state execution is paused or not. */
        auto property_paused() { return _prop_paused.get_proxy(); }

        /** Whether the Lua state execution is paused or not. */
        auto property_paused() const { return _prop_paused.get_proxy(); }

        /**
         * @return Whether the Lua state execution is paused or not.
         */
        bool is_paused() const { return property_paused().get_value(); }

        /**
         * Pause or unpause Lua state execution.
         *
         * @param pause Whether the Lua state execution should be paused or not.
         */
        void set_pause(bool pause) { property_paused().set_value(pause); }

        /** Update. */
        void update();

    protected:
        void on_lua_state_changed();
        void on_pause_resume_clicked();
        void on_paused_changed();

    private:
        Sickle::LuaDebugger _debugger{};

        Gtk::Box _box{Gtk::Orientation::ORIENTATION_VERTICAL};

        Gtk::ToolButton _pause_resume_button{};
        Gtk::Toolbar _tools{};

        Glib::Property<gpointer> _prop_lua_state;
        Glib::Property<gboolean> _prop_paused;
        Glib::RefPtr<Glib::Binding> _bind_lua_state_debugger;
    };
} // namespace Sickle

#endif
