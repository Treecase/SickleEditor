/**
 * CallStackInspector.hpp - Lua function call inspection widget.
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

#ifndef SE_LUADEBUGGER_CALLSTACKINSPECTOR_HPP
#define SE_LUADEBUGGER_CALLSTACKINSPECTOR_HPP

#include <se-lua/se-lua.hpp>

#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

namespace Sickle
{
    /**
     * Lists a Lua state's current stack frames.
     *
     * The currently selected frame is available in the "level" property.
     */
    class CallStackInspector : public Gtk::Bin
    {
    public:
        CallStackInspector();

        auto property_lua_state() { return _prop_lua_state.get_proxy(); }

        auto property_lua_state() const { return _prop_lua_state.get_proxy(); }

        void set_lua_state(lua_State *L) { property_lua_state().set_value(L); }

        lua_State *get_lua_state() const
        {
            return static_cast<lua_State *>(property_lua_state().get_value());
        }

        /**
         * The currently selected call stack level. 0 is the currently running
         * function, 1 is the caller of the current function, and so on.
         */
        auto property_level() const { return _prop_level.get_proxy(); }

        /**
         * The currently selected call stack level. 0 is the currently running
         * function, 1 is the caller of the current function, and so on.
         */
        auto property_level() { return _prop_level.get_proxy(); }

        /**
         * Get the selected call stack level.
         *
         * @return The selected stack level.
         */
        int get_level() const { return property_level().get_value(); }

        void update();
        void clear();

    protected:
        void on_selection_changed();

    private:
        struct ModelColumn : public Gtk::TreeModelColumnRecord
        {
            ModelColumn()
            {
                add(level);
                add(name);
            }

            Gtk::TreeModelColumn<int> level;
            Gtk::TreeModelColumn<std::string> name;
        };

        Glib::Property<gpointer> _prop_lua_state;
        Glib::Property<int> _prop_level;

        ModelColumn _columns{};
        Glib::RefPtr<Gtk::ListStore> _store{nullptr};
        Gtk::TreeView _view;
    };
} // namespace Sickle

#endif
