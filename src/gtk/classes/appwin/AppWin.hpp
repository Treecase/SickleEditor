/**
 * AppWin.hpp - Sickle ApplicationWindow.
 * Copyright (C) 2022-2023 Trevor Last
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

#ifndef SE_APPWIN_HPP
#define SE_APPWIN_HPP

#include "../LuaConsole.hpp"
#include "../maparea2d/MapArea2D.hpp"
#include "../MapArea3D.hpp"
#include "../MapTools.hpp"
#include "OperationSearch.hpp"

#include <core/Editor.hpp>
#include <operations/Operation.hpp>
#include <se-lua/utils/Referenceable.hpp>
#include <appid.hpp>
#include <AppWin_Lua.hpp>

#include <glibmm/property.h>
#include <glibmm/binding.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/infobar.h>
#include <gtkmm/label.h>
#include <gtkmm/searchentry.h>

#include <functional>


namespace Sickle::AppWin
{
    class AppWin : public Gtk::ApplicationWindow, public Lua::Referenceable
    {
    public:
        lua_State *const L;
        Glib::RefPtr<Editor::Editor> editor;

        AppWin();

        /** Open a file. */
        void open(Glib::RefPtr<Gio::File> const &file);
        /** Save currently edited map. */
        void save(std::string const &filename);
        /** Open the Lua console window. */
        void show_console_window();
        /** Reload Lua scripts. */
        void reload_scripts();
        /** Open the Operation Search dialog. */
        void search_operations();

        /** Add a MapTool. */
        void add_maptool(Editor::MapTool const &maptool);

        void set_grid_size(guint grid_size);
        guint get_grid_size();

        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        auto signal_lua_reloaded() {return _sig_lua_reloaded;}

        // Lua constructor needs access to private members.
        friend void Lua::push(lua_State *, AppWin *);

    protected:
        void setup_lua_state();

        void on_action_openLuaConsole();
        void on_action_reloadLua();

        bool on_key_press_event(GdkEventKey *event) override;

    private:
        static constexpr guint GRID_SIZE_MIN = 1;
        static constexpr guint GRID_SIZE_MAX = 512;

        // Important widgets
        MapArea3D _view3d;
        MapArea2D _view2d_top, _view2d_front, _view2d_right;
        MapTools _maptools;
        Gtk::Label _gridsizelabel{};
        Gtk::Window _luaconsolewindow{};
        LuaConsole _luaconsole{};
        Gtk::InfoBar _luainfobar{};
        OperationSearch _opsearch;

        // Structural widgets
        Gtk::Grid _basegrid{};
        Gtk::Grid _viewsgrid{};
        Gtk::HBox _inforegion{};
        Gtk::Label _luainfobarlabel{};

        Glib::Property<guint> _prop_grid_size;
        Glib::RefPtr<Glib::Binding> _binding_grid_size_top,
            _binding_grid_size_front,
            _binding_grid_size_right;
        sigc::signal<void()> _sig_lua_reloaded{};

        std::vector<std::string> _lua_script_dirs{
            SE_DATA_DIR "sickle/lua-runtime",
            "../share/sickle/lua-runtime",
        };
        std::vector<std::string> _operation_script_dirs{
            SE_DATA_DIR "sickle/operations",
            "../share/sickle/operations",
        };
        std::vector<std::string> const _internal_scripts{
            "lua/gdkevents.lua",
            "lua/gdkkeysyms.lua",
            "lua/gdktypes.lua",
        };

        std::vector<Editor::MapTool> _predefined_maptools{};

        static std::vector<std::string> _filter_dirs(
            std::vector<std::string> const &dirs);
        static std::vector<std::string> _find_scripts_in_dirs(
            std::vector<std::string> const &dirs);
        static std::string _make_lua_include_path(std::string const &dir);

        void _on_grid_size_changed();
        void _on_opsearch_op_chosen(Editor::Operation const &op);

        void _run_internal_scripts();
        void _run_runtime_scripts();
        void _run_operations_scripts();
    };
}

#endif
