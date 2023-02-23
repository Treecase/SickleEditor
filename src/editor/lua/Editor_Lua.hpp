/**
 * Editor_Lua.hpp - Editor Lua binding.
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

#ifndef SE_EDITOR_LUA_HPP
#define SE_EDITOR_LUA_HPP

#include "se-lua/se-lua.hpp"


namespace Sickle
{
    class Editor;
}

int luaopen_editor(lua_State *L);
int leditor_new(lua_State *L, Sickle::Editor *editor);
Sickle::Editor *leditor_check(lua_State *L, int arg);

int luaopen_selection(lua_State *L);
int lselection_new(lua_State *L, Sickle::Editor::Selection *selection);
Sickle::Editor::Selection *lselection_check(lua_State *L, int arg);

int luaopen_brushbox(lua_State *L);
int lbrushbox_new(lua_State *L, Sickle::Editor::BrushBox *selection);
Sickle::Editor::BrushBox *lbrushbox_check(lua_State *L, int arg);

int luaopen_editorbrush(lua_State *L);
int leditorbrush_new(lua_State *L, Sickle::EditorBrush *brush);
Sickle::EditorBrush *leditorbrush_check(lua_State *L, int arg);

#endif
