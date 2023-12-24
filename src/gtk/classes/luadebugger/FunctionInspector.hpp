/**
 * FunctionInspector.hpp - Lua function call inspection widget.
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

#ifndef SE_LUADEBUGGER_FUNCTIONINSPECTOR_HPP
#define SE_LUADEBUGGER_FUNCTIONINSPECTOR_HPP

#include <se-lua/se-lua.hpp>

#include <glibmm/property.h>
#include <gtkmm/box.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>


namespace Sickle
{
    /**
     * The FunctionInspector displays a function's local variables.
     *
     * The function to inspect is selected by the "level" property.
     */
    class FunctionInspector : public Gtk::Box
    {
    public:
        FunctionInspector();

        auto property_lua_state() {return _prop_lua_state.get_proxy();}
        auto property_lua_state() const {return _prop_lua_state.get_proxy();}
        void set_lua_state(lua_State *L) {property_lua_state().set_value(L);}
        lua_State *get_lua_state() const {
            return static_cast<lua_State *>(property_lua_state().get_value());}

        /** Lua call stack level to be inspected. */
        auto property_level() {return _prop_level.get_proxy();}
        /** Lua call stack level to be inspected. */
        auto property_level() const {return _prop_level.get_proxy();}
        /**
         * Set the call stack level to inspect.
         *
         * @todo Limit the value to valid stack levels.
         * @param level The new stack level.
         */
        void set_level(int level) {property_level().set_value(level);}
        /**
         * Get the stack level of the function currently being inspected.
         *
         * @return The inspected stack level.
         */
        int get_level() const {return property_level().get_value();}

        void update();
        void clear();

    private:
        struct ModelColumn : public Gtk::TreeModelColumnRecord
        {
            ModelColumn()
            {
                add(level);
                add(display);
            }

            Gtk::TreeModelColumn<int> level;
            Gtk::TreeModelColumn<std::string> display;
        };

        Glib::Property<gpointer> _prop_lua_state;
        Glib::Property<int> _prop_level;

        ModelColumn _columns{};
        Glib::RefPtr<Gtk::TreeStore> _store{nullptr};
        Glib::RefPtr<Gtk::TreeModelFilter> _filtered{nullptr};
        Gtk::TreeView _view;

        void on_level_changed();
    };
}

#endif
