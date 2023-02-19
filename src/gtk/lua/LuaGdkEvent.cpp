/**
 * GdkEvent_Lua.cpp - Lua::Pusher specializations for GDK events.
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

#include "LuaGdkEvent.hpp"


template<>
void Lua::Pusher::operator()(GdkEventKey const *event)
{
    lua_pushinteger(L, (lua_Integer)event->keyval);
}

template<>
void Lua::Pusher::operator()(GdkEventButton const *event)
{
    make_table(L,
        std::make_pair("x", event->x),
        std::make_pair("y", event->y),
        std::make_pair("button", (lua_Integer)event->button));
}

template<>
void Lua::Pusher::operator()(GdkEventMotion const *event)
{
    make_table(L,
        std::make_pair("state", (lua_Integer)event->state),
        std::make_pair("x", event->x),
        std::make_pair("y", event->y));
}

template<>
void Lua::Pusher::operator()(GdkEventScroll const *event)
{
    make_table(L, std::make_pair("direction", (lua_Integer)event->direction));
}
