/**
 * Editor_Lua.hpp - Editor Lua binding.
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

#ifndef SE_EDITOR_LUA_HPP
#define SE_EDITOR_LUA_HPP

#include "editor/Editor.hpp"

#include <se-lua/se-lua.hpp>


int luaopen_editor(lua_State *L);
Sickle::Editor::Editor *leditor_check(lua_State *L, int arg);

int luaopen_selection(lua_State *L);
Sickle::Editor::Selection *lselection_check(lua_State *L, int arg);

int luaopen_brushbox(lua_State *L);
Sickle::Editor::BrushBox *lbrushbox_check(lua_State *L, int arg);

int luaopen_editorbrush(lua_State *L);
Sickle::Editor::Brush *leditorbrush_check(lua_State *L, int arg);

template<> void Lua::push(lua_State *L, Sickle::Editor::Editor *editor);
template<> void Lua::push(lua_State *L, Sickle::Editor::Selection *selection);
template<> void Lua::push(lua_State *L, Sickle::Editor::BrushBox *brushbox);
template<> void Lua::push(lua_State *L, Sickle::Editor::Brush *brush);

#endif
