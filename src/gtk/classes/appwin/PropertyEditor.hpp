/**
 * PropertyEditor.hpp - The Sickle object property editor.
 * Copyright (C) 2024 Trevor Last
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

#ifndef SE_APPWIN_PROPERTY_EDITOR_HPP
#define SE_APPWIN_PROPERTY_EDITOR_HPP

#include <editor/world/Entity.hpp>

#include <gtkmm/bin.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>


namespace Sickle::AppWin
{
    class PropertyEditor : public Gtk::Bin
    {
    public:
        PropertyEditor();

        auto property_entity() {return _prop_entity.get_proxy();}
        auto property_entity() const {return _prop_entity.get_proxy();}
        void set_entity(Editor::EntityRef const &entity);
        Editor::EntityRef get_entity() const;

    protected:
        void on_entity_changed();
        void on_name_edited(
            Glib::ustring const &path,
            Glib::ustring const &new_name);
        void on_value_edited(
            Glib::ustring const &path,
            Glib::ustring const &new_value);

    private:
        struct Columns : Gtk::TreeModelColumnRecord
        {
            Columns()
            {
                add(name);
                add(value);
            }

            Gtk::TreeModelColumn<Glib::ustring> name{};
            Gtk::TreeModelColumn<Glib::ustring> value{};
        };

        Glib::Property<Editor::EntityRef> _prop_entity;

        Gtk::Frame _frame{};
        // scroll, buttons
        Gtk::Box _main_box{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::ScrolledWindow _scroll{};
        Columns _columns{};
        Glib::RefPtr<Gtk::ListStore> _store{};
        Gtk::TreeView _properties;

        // add property, remove property
        Gtk::Box _buttons_box{Gtk::Orientation::ORIENTATION_HORIZONTAL};
        Gtk::Button _add_property_button{};
        Gtk::Button _remove_property_button{};

        void _add_property();
        void _remove_property();
    };
}

#endif
