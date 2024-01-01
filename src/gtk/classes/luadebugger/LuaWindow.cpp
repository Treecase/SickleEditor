/**
 * LuaWindow.cpp - Window containing Lua debugging functionality.
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

#include "LuaWindow.hpp"

using namespace Sickle;


static void line_hook(lua_State *L, lua_Debug *dbg)
{
    if (!lua_checkstack(L, 1))
        return;
    lua_getfield(L, LUA_REGISTRYINDEX, "__debugger");
    auto debugger = static_cast<LuaWindow *>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    if (debugger->is_paused())
        debugger->update();
}


LuaWindow::LuaWindow()
:   Glib::ObjectBase{typeid(LuaWindow)}
,   Gtk::Window{}
,   _prop_lua_state{*this, "lua-state", nullptr}
,   _prop_paused{*this, "paused", false}
,   _bind_lua_state_debugger{
        Glib::Binding::bind_property(
            property_lua_state(),
            _debugger.property_lua_state(),
            Glib::BindingFlags::BINDING_SYNC_CREATE)}
{
    _pause_resume_button.set_icon_name("media-playback-pause");
    _pause_resume_button.signal_clicked().connect(
        sigc::mem_fun(*this, &LuaWindow::on_pause_resume_clicked));
    _tools.append(_pause_resume_button);

    _box.add(_tools);
    _box.add(_debugger);
    add(_box);

    set_size_request(640, 480);

    show_all_children();

    property_lua_state().signal_changed().connect(
        sigc::mem_fun(*this, &LuaWindow::on_lua_state_changed));
    property_paused().signal_changed().connect(
        sigc::mem_fun(*this, &LuaWindow::on_paused_changed));
}


void LuaWindow::update()
{
    _debugger.on_error();
}



void LuaWindow::on_lua_state_changed()
{
    auto const L = get_lua_state();
    if (L)
    {
        if (!lua_checkstack(L, 1))
            throw Lua::StackOverflow{};
        lua_pushlightuserdata(L, this);
        lua_setfield(L, LUA_REGISTRYINDEX, "__debugger");
        lua_sethook(L, line_hook, LUA_MASKLINE, 0);
    }
}


void LuaWindow::on_pause_resume_clicked()
{
    set_pause(!is_paused());
}


void LuaWindow::on_paused_changed()
{
    if (is_paused())
        _pause_resume_button.set_icon_name("media-playback-start");
    else
        _pause_resume_button.set_icon_name("media-playback-pause");
}
