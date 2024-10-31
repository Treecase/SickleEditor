/**
 * LuaDebugger.hpp - Lua debugging widget.
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

#ifndef SE_LUADEBUGGER_HPP
#define SE_LUADEBUGGER_HPP

#include "CallStackInspector.hpp"
#include "FunctionInspector.hpp"
#include "StackInspector.hpp"

#include <se-lua/se-lua.hpp>

#include <glibmm/binding.h>
#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>

namespace Sickle
{
    /**
     * Debugging widget for Lua states.
     *
     * Includes inspectors for the stack, the call stack, and local function
     * variables.
     */
    class LuaDebugger : public Gtk::Box
    {
    public:
        LuaDebugger();
        virtual ~LuaDebugger();

        auto property_lua_state() { return _prop_lua_state.get_proxy(); }

        auto property_lua_state() const { return _prop_lua_state.get_proxy(); }

        void set_lua_state(lua_State *L) { property_lua_state().set_value(L); }

        lua_State *get_lua_state() const
        {
            return static_cast<lua_State *>(property_lua_state().get_value());
        }

        /**
         * Update child widgets to display information about an error.
         *
         * @todo Better way to do this/better name
         */
        void on_error();

    private:
        Glib::Property<gpointer> _prop_lua_state;

        FunctionInspector _function_inspector{};
        CallStackInspector _call_stack_inspector{};
        StackInspector _stack_inspector{};

        Gtk::Box _call_box{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::ScrolledWindow _function_inspector_scroll{};
        Gtk::ScrolledWindow _call_stack_inspector_scroll{};
        Gtk::ScrolledWindow _stack_inspector_scroll{};

        Glib::RefPtr<Glib::Binding> _bind_lua_state_function_inspector{nullptr};
        Glib::RefPtr<Glib::Binding> _bind_lua_state_call_stack_inspector{
            nullptr};
        Glib::RefPtr<Glib::Binding> _bind_lua_state_stack_inspector{nullptr};
        Glib::RefPtr<Glib::Binding> _bind_level_stackinsp_funcinsp{nullptr};
    };
} // namespace Sickle

#endif
