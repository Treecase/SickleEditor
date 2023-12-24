/**
 * LuaDebugger.cpp - Lua debugging widget.
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

#include "LuaDebugger.hpp"

using namespace Sickle;


LuaDebugger::LuaDebugger()
:   Glib::ObjectBase{typeid(LuaDebugger)}
,   Gtk::Box{Gtk::Orientation::ORIENTATION_HORIZONTAL}
,   _prop_lua_state{*this, "lua-state", nullptr}
,   _bind_lua_state_function_inspector{Glib::Binding::bind_property(
        property_lua_state(),
        _function_inspector.property_lua_state(),
        Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _bind_lua_state_call_stack_inspector{Glib::Binding::bind_property(
        property_lua_state(),
        _call_stack_inspector.property_lua_state(),
        Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _bind_lua_state_stack_inspector{Glib::Binding::bind_property(
        property_lua_state(),
        _stack_inspector.property_lua_state(),
        Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _bind_level_stackinsp_funcinsp{Glib::Binding::bind_property(
        _call_stack_inspector.property_level(),
        _function_inspector.property_level(),
        Glib::BindingFlags::BINDING_SYNC_CREATE)}
{
    _function_inspector_scroll.add(_function_inspector);
    _call_stack_inspector_scroll.add(_call_stack_inspector);

    _call_box.add(_function_inspector_scroll);
    _call_box.add(_call_stack_inspector_scroll);
    add(_call_box);

    _stack_inspector_scroll.add(_stack_inspector);
    add(_stack_inspector_scroll);
}


LuaDebugger::~LuaDebugger()
{
}


void LuaDebugger::on_error()
{
    _function_inspector.update();
    _call_stack_inspector.update();
    _stack_inspector.update();
}
