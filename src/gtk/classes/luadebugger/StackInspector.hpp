/**
 * StackInspector.hpp - Lua stack inspector widget.
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

#ifndef SE_LUADEBUGGER_STACKINSPECTOR_HPP
#define SE_LUADEBUGGER_STACKINSPECTOR_HPP

#include <se-lua/se-lua.hpp>

#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>


namespace Sickle
{
    /**
     * Allows the user to inspect a Lua stack.
     *
     * Displays a table containing all the values on the stack
     */
    class StackInspector : public Gtk::Bin
    {
    public:
        StackInspector();

        auto property_lua_state() {return _prop_lua_state.get_proxy();}
        auto property_lua_state() const {return _prop_lua_state.get_proxy();}
        void set_lua_state(lua_State *L) {property_lua_state().set_value(L);}
        lua_State *get_lua_state() const {
            return static_cast<lua_State *>(property_lua_state().get_value());}

        /**
         * Update the inspector's stack view with the current stack state.
         */
        void update();

        /**
         * Clear the inspector's stack view.
         */
        void clear();

    protected:
        void on_lua_state_changed();

    private:
        Glib::Property<gpointer> _prop_lua_state;

        struct ModelColumn : public Gtk::TreeModelColumnRecord
        {
            ModelColumn()
            {
                add(index);
                add(type);
                add(types);
                add(value);
            }

            Gtk::TreeModelColumn<int> index{};
            Gtk::TreeModelColumn<std::string> type{};
            Gtk::TreeModelColumn<Glib::RefPtr<Gtk::TreeModel>> types{};
            Gtk::TreeModelColumn<std::string> value{};
        };

        struct ComboColumns : public Gtk::TreeModelColumnRecord
        {
            ComboColumns()
            {
                add(types);
            }
            Gtk::TreeModelColumn<Glib::ustring> types{};
        };

        ModelColumn _columns{};
        ComboColumns _combo_columns{};
        Glib::RefPtr<Gtk::ListStore> _store{nullptr};
        Glib::RefPtr<Gtk::ListStore> _combo_store{nullptr};
        Gtk::TreeView _view{};

        void _on_type_cell_edited(
            Glib::ustring const &path,
            Glib::ustring const &value);
        void _update_store();
    };
}

#endif
