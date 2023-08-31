/**
 * SelectMenu.cpp - Popup menu for "Select" tool.
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

#include "SelectMenu.hpp"

#include <gtkmm/builder.h>
#include <giomm/menu.h>
#include <appid.hpp>


using namespace Sickle;


static Glib::RefPtr<Gio::Menu> load_menu_resource()
{
    auto builder = Gtk::Builder::create();
    builder->add_from_resource(
        SE_GRESOURCE_PREFIX"gtk/MapArea2D/SelectPopupMenu.ui");
    return Glib::RefPtr<Gio::Menu>::cast_dynamic(
        builder->get_object("popup-select"));
}


SelectMenu::SelectMenu()
:   Glib::ObjectBase{typeid(SelectMenu)}
,   Gtk::Menu{load_menu_resource()}
,   _actions{Gio::SimpleActionGroup::create()}
{
    _actions->add_action(
        "delete",
        sigc::mem_fun(*this, &SelectMenu::action_delete));
    insert_action_group("select", _actions);
}


void SelectMenu::set_editor(decltype(_editor) editor)
{
    _editor = editor;
}



void SelectMenu::action_delete()
{
    if (!_editor)
        return;

    std::vector<Editor::Selection::Item> const cached{
        _editor->selected.begin(),
        _editor->selected.end()};
    _editor->selected.clear();
    for (auto const &brush : cached)
        _editor->get_map()->remove_brush(brush);
}
