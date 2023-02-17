/**
 * LuaConsole.cpp - Sickle editor Lua console.
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

#include "LuaConsole.hpp"
#include "AppWin.hpp"

#include <gtkmm.h>

#include <sstream>


int _customprint(lua_State *L)
{
    lua_getglobal(L, "_console");
    auto console = static_cast<Sickle::LuaConsole *>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return console->_L_customprint(L);
}



Sickle::LuaConsole::LuaConsole()
:   Glib::ObjectBase{typeid(LuaConsole)}
,   Gtk::Box{}
,   m_scroll{}
,   m_output{}
,   m_input{}
,   _prop_lua_state{*this, "lua-state", nullptr}
,   L{nullptr}
,   _fp{nullptr}
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
}

void Sickle::LuaConsole::writeline(std::string const &str)
{
    write(str + "\n");
}


void Sickle::LuaConsole::on_input_activated()
{
    writeline(">>> " + m_input.get_text());
    try
    {
        Lua::checkerror(L, luaL_dostring(L, m_input.get_text().c_str()));
    }
    catch (Lua::Error const &e)
    {
        try
        {
            Lua::checkerror(L, luaL_dostring(L, ("print(" + m_input.get_text() + ")").c_str()));
        }
        catch (Lua::Error const &)
        {
            writeline(e.what());
        }
    }
    m_input.set_text("");
}


void Sickle::LuaConsole::_on_lua_state_changed()
{
    L = static_cast<lua_State *>(property_lua_state().get_value());

    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "_console");
    // Override Lua's "print"
    static struct luaL_Reg const printlib[] = {
        {"print", _customprint}, {NULL, NULL}};
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);
}

int Sickle::LuaConsole::_L_customprint(lua_State *L)
{
    std::stringstream str{};
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i)
    {
        switch (lua_type(L, i))
        {
        case LUA_TBOOLEAN:
            str << (lua_toboolean(L, i)? "true" : "false");
            break;
        case LUA_TFUNCTION:
            str << "<function>";
            break;
        case LUA_TLIGHTUSERDATA:
            str << lua_touserdata(L, i);
            break;
        case LUA_TNIL:
            str << "nil";
            break;
        case LUA_TNONE:
            str << "<none>";
            break;
        case LUA_TNUMBER:
            str << lua_tonumber(L, i);
            break;
        case LUA_TSTRING:
            str << lua_tostring(L, i);
            break;
        case LUA_TTABLE:
            str << "<table>";
            break;
        case LUA_TTHREAD:
            str << "<thread>";
            break;
        case LUA_TUSERDATA:
            str << lua_touserdata(L, i);
            break;
        default:
            str << '?';
            break;
        }
    }
    writeline(str.str());
    return 0;
}
