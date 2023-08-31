/**
 * CreateBrushMenu.cpp - Popup menu for "Create Brush" tool.
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

#include "CreateBrushMenu.hpp"

#include <gtkmm/builder.h>
#include <giomm/menu.h>
#include <appid.hpp>

#include <iostream>


using namespace Sickle;


static Glib::RefPtr<Gio::Menu> load_menu_resource()
{
    auto builder = Gtk::Builder::create();
    builder->add_from_resource(
        SE_GRESOURCE_PREFIX"gtk/MapArea2D/CreateBrushPopupMenu.ui");
    return Glib::RefPtr<Gio::Menu>::cast_dynamic(
        builder->get_object("popup-createbrush"));
}



CreateBrushMenu::CreateBrushMenu()
:   Glib::ObjectBase{typeid(CreateBrushMenu)}
,   Gtk::Menu{load_menu_resource()}
,   _actions{Gio::SimpleActionGroup::create()}
{
    _actions->add_action(
        "create",
        sigc::mem_fun(*this, &CreateBrushMenu::action_create));
    insert_action_group("createbrush", _actions);
}


void CreateBrushMenu::set_editor(decltype(_editor) editor)
{
    _editor = editor;
}



void CreateBrushMenu::action_create()
{
    if (!_editor)
        return;
    try {
        _editor->do_command(std::make_shared<Editor::commands::AddBrush>());
    }
    catch (std::runtime_error const &e) {
        std::cout
            << "Sickle::MapArea2D::on_action_createbrush_create -- "
            << e.what()
            << '\n';
    }
}
