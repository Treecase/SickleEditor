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

#ifndef SE_APPWIN_PROPERTYEDITOR_HPP
#define SE_APPWIN_PROPERTYEDITOR_HPP

#include "CellRendererProperty.hpp"

#include <editor/world/Entity.hpp>

#include <gtkmm/bin.h>
#include <gtkmm/frame.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>

namespace Sickle::AppWin
{
    /**
     * Allows the user to modify Entity properties.
     *
     * Displays the entity's properties in an editable TreeView. The user can
     * change the value of properties. Properties of different types are
     * displayed using an appropriate CellRenderer, so eg. a string uses a
     * CellRendererText while a number uses a CellRendererSpin.
     */
    class PropertyEditor : public Gtk::Bin
    {
    public:
        PropertyEditor();

        /** The entity currently being modified. */
        auto property_entity() { return _prop_entity.get_proxy(); }

        /** The entity currently being modified. */
        auto property_entity() const { return _prop_entity.get_proxy(); }

        /**
         * Change the entity to be edited.
         *
         * @param entity The entity to edit.
         */
        void set_entity(Editor::EntityRef const &entity);
        /**
         * Get the entity being edited.
         *
         * @return The entity being edited.
         */
        Editor::EntityRef get_entity() const;

    protected:
        void on_entity_changed();
        void on_entity_properties_changed();
        void on_value_edited(
            Glib::ustring const &path,
            Glib::ustring const &new_value);

    private:
        struct Columns : Gtk::TreeModelColumnRecord
        {
            Columns()
            {
                add(name);
                add(renderer_value);
                add(tooltip);
                add(choices);
            }

            Gtk::TreeModelColumn<Glib::ustring> name{};
            Gtk::TreeModelColumn<CellRendererProperty::ValueType>
                renderer_value{};
            Gtk::TreeModelColumn<Glib::ustring> tooltip{};
            Gtk::TreeModelColumn<Glib::RefPtr<Gtk::ListStore>> choices{};
        };

        Glib::Property<Editor::EntityRef> _prop_entity;
        sigc::connection _conn_entity_properties_changed{};

        Glib::RefPtr<Gtk::ListStore> _store{};
        CellRendererProperty _renderer{};

        Gtk::Frame _frame{};
        Gtk::ScrolledWindow _scroll{};
        Gtk::TreeView _properties;

        static Columns const &_columns();

        void _update_row(
            Gtk::TreeModel::iterator const &it,
            Glib::ustring const &name,
            std::string const &value,
            std::shared_ptr<Sickle::Editor::EntityPropertyDefinition> const
                &property_definition);

        Glib::ustring _choices_filter_edit(
            Glib::ustring const &path,
            Glib::ustring const &choice) const;
    };
} // namespace Sickle::AppWin

#endif
