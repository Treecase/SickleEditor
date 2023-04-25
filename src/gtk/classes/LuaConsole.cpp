/**
 * LuaConsole.cpp - Sickle editor Lua console.
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

#include "LuaConsole.hpp"
#include "AppWin.hpp"

#include <gtkmm.h>

#include <iostream>
#include <sstream>


static int _customprint(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "_console");
    auto console = static_cast<Sickle::LuaConsole *>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return console->_print_override();
}

static void debug_hook(lua_State *L, lua_Debug *ar)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "_console");
    auto console = static_cast<Sickle::LuaConsole *>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    console->_debug_hook(ar);
}


Sickle::LuaConsole::LuaConsole()
:   Glib::ObjectBase{typeid(LuaConsole)}
,   Gtk::Box{}
,   m_scroll{}
,   m_output{}
,   m_input{}
,   _prop_lua_state{*this, "lua-state", nullptr}
{
    property_lua_state().signal_changed().connect(
        sigc::mem_fun(*this, &LuaConsole::_on_lua_state_changed));
    m_input.signal_activate().connect(
        sigc::mem_fun(*this, &LuaConsole::on_input_activated));

    set_hexpand(true);
    set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    set_size_request(80, 80);
    pack_start(m_scroll);
    add(m_input);

    auto css = Gtk::CssProvider::create();
    css->load_from_resource(SE_GRESOURCE_PREFIX "LuaConsole.css");
    m_input.get_style_context()->add_provider_for_screen(
        Gdk::Screen::get_default(),
        css,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    m_output.get_style_context()->add_provider_for_screen(
        Gdk::Screen::get_default(),
        css,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    m_output.set_editable(false);
    m_output.set_monospace(true);
    m_output.set_can_focus(false);

    m_scroll.add(m_output);

    show_all_children();
}

void Sickle::LuaConsole::write(std::string const &str)
{
    m_output.get_buffer()->insert(m_output.get_buffer()->end(), str);
    auto it = m_output.get_buffer()->end();
    m_output.scroll_to(it);
}

void Sickle::LuaConsole::writeline(std::string const &str)
{
    write(str + "\n");
}

int Sickle::LuaConsole::_print_override()
{
    std::stringstream str{};
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i)
    {
        str << luaL_tolstring(L, i, nullptr);
        lua_pop(L, 1);
        if (i != nargs)
            str << ' ';
    }
    writeline(str.str());
    std::cout << str.str() << '\n';
    return 0;
}

void Sickle::LuaConsole::_debug_hook(lua_Debug *ar)
{
    lua_getinfo(L, "lS", ar);
    debug.line_number = ar->currentline;
    debug.where = ar->short_src;
}


void Sickle::LuaConsole::error_handler(lua_State *s)
{
    std::string error_string{lua_tostring(L, -1)};
    lua_pop(L, 1);
    std::stringstream errstr{};
    errstr << "ERROR (" << debug.where << ':' << debug.line_number << ')'
        << " -- " << error_string;
    writeline(errstr.str());
    std::cerr << errstr.str() << '\n';
}

void Sickle::LuaConsole::on_input_activated()
{
    writeline(">>> " + m_input.get_text());
    auto pre = lua_gettop(L);
    int status = luaL_dostring(L, m_input.get_text().c_str());
    if (status != LUA_OK)
    {
        int status = luaL_dostring(L,
            ("print(" + m_input.get_text() + ")").c_str());
        if (status != LUA_OK)
        {
            auto error_string = lua_tostring(L, -1);
            lua_pop(L, 1);
            writeline(error_string);
        }
    }
    lua_pop(L, lua_gettop(L) - pre);
    m_input.set_text("");
}


void Sickle::LuaConsole::_on_lua_state_changed()
{
    // Reset previously held lua_State.
    if (L != nullptr)
    {
        Lua::clear_error_handler(L);

        lua_pushnil(L);
        lua_setfield(L, LUA_REGISTRYINDEX, "_console");

        lua_getglobal(L, "_G");
        lua_pushlightuserdata(L, this);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_setfield(L, -2, "print");
        lua_pop(L, 1);
    }

    L = static_cast<lua_State *>(property_lua_state().get_value());

    Lua::set_error_handler(L, std::bind(
        &Sickle::LuaConsole::error_handler, this, std::placeholders::_1));

    lua_sethook(L, debug_hook, LUA_MASKLINE, 0);

    lua_pushlightuserdata(L, this);
    lua_setfield(L, LUA_REGISTRYINDEX, "_console");
    // Save the old print function
    lua_pushlightuserdata(L, this);
    lua_getglobal(L, "print");
    lua_settable(L, LUA_REGISTRYINDEX);
    // Override Lua's "print"
    lua_getglobal(L, "_G");
    lua_pushcfunction(L, _customprint);
    lua_setfield(L, -2, "print");
    lua_pop(L, 1);
}
