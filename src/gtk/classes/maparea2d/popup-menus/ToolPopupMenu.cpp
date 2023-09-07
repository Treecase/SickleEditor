/**
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

#include "ToolPopupMenu.hpp"

#include <giomm/menu.h>

#include <iostream> // FIXME: temp?


using namespace Sickle;


static auto nospace_lower(Glib::ustring const &string)
{
    Glib::ustring out{};
    for (auto const ch : string.lowercase())
        if (!g_unichar_isspace(ch))
            out += ch;
    return out;
}


static Glib::RefPtr<Gio::Menu> create_menu(Editor::MapTool const &tool)
{
    auto menu = Gio::Menu::create();
    for (auto const &op : tool.operations())
    {
        menu->append(op.label, nospace_lower(tool.name() + "." + op.label));
    }
    return menu;
}


ToolPopupMenu::ToolPopupMenu(Editor::MapTool const &tool)
:   Glib::ObjectBase{typeid(ToolPopupMenu)}
,   Gtk::Menu{create_menu(tool)}
,   _actions{Gio::SimpleActionGroup::create()}
,   _tool{tool}
{
    for (auto const &op : _tool.operations())
    {
        _actions->add_action(
            nospace_lower(op.label),
            sigc::bind(
                sigc::mem_fun(*this, &ToolPopupMenu::action_triggered),
                op.operation_id));
    }
    insert_action_group(nospace_lower(_tool.name()), _actions);
}


void ToolPopupMenu::set_editor(decltype(_editor) editor)
{
    _editor = editor;
}


bool ToolPopupMenu::should_popup() const
{
    return _tool.x(_editor);
}



void ToolPopupMenu::action_triggered(std::string const &id)
{
    if (!_editor)
        return;

    try {
        auto const op = _editor->oploader->get_operation(id);
        op.execute(_editor, {});
    }
    catch (Lua::Error const &e) {
        std::cerr << e.what() << '\n'; // FIXME: temp?
    }
}
