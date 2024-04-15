/**
 * AppWin.hpp - Sickle ApplicationWindow.
 * Copyright (C) 2022-2024 Trevor Last
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

#include "../maparea2d/MapArea2D.hpp"
#include "../MapArea3D.hpp"
#include "FaceEditor.hpp"
#include "LuaWindow.hpp"
#include "LuaConsole.hpp"
#include "MapToolConfig.hpp"
#include "MapTools.hpp"
#include "ModeSelector.hpp"
#include "OperationSearch.hpp"
#include "Outliner.hpp"
#include "propertyeditor/PropertyEditor.hpp"

#include <AppWin_Lua.hpp>
#include <config/appid.hpp>
#include <editor/core/Editor.hpp>
#include <editor/operations/Operation.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <glibmm/property.h>
#include <glibmm/binding.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/infobar.h>
#include <gtkmm/label.h>
#include <gtkmm/overlay.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/stackswitcher.h>

#include <functional>


namespace Sickle::AppWin
{
    class AppWin : public Gtk::ApplicationWindow, public Lua::Referenceable
    {
    public:
        lua_State *const L;
        Editor::EditorRef editor;

        AppWin();
        virtual ~AppWin()=default;

        /** Open a file. */
        void open(Glib::RefPtr<Gio::File> const &file);
        /** Save currently edited map. */
        void save(std::string const &filename);
        /** Open the Lua console window. */
        void show_console_window();
        /** Open the Lua debugging window. */
        void show_debugger_window();
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

        void on_maptoolconfig_confirmed();

        void on_action_openLuaConsole();
        void on_action_openLuaDebugger();
        void on_action_reloadLua();

        virtual bool on_key_press_event(GdkEventKey *event) override;

    private:
        static constexpr guint GRID_SIZE_MIN = 1;
        static constexpr guint GRID_SIZE_MAX = 512;

        // Important widgets
        MapArea3D _view3d;
        MapArea2D _view2d_top, _view2d_front, _view2d_right;
        MapTools _maptools;
        MapToolConfig _maptool_config;
        Gtk::Label _gridsizelabel{};
        Gtk::Window _lua_console_window{};
        LuaConsole _lua_console{};
        LuaWindow _lua_debugger_window{};
        Gtk::InfoBar _luainfobar{};
        OperationSearch *_opsearch{nullptr};
        ModeSelector _mode_selector{};
        Outliner _outliner{};
        PropertyEditor _property_editor{};
        FaceEditor _face_editor;

        // Structural widgets
        Gtk::Grid _basegrid{};
        Gtk::HBox _inforegion{};
        Gtk::Label _luainfobarlabel{};
        Gtk::Box _stack_container{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::Stack _object_editor_stack{};
        Gtk::StackSwitcher _object_editor_switcher{};
        Gtk::ScrolledWindow _face_edit_scroll{};

        // 3D View, 2D View (Front)
        Gtk::Paned _left_views{Gtk::Orientation::ORIENTATION_VERTICAL};
        // 2D View (Top), 2D View (right)
        Gtk::Paned _right_views{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::Paned _views{Gtk::Orientation::ORIENTATION_HORIZONTAL};

        // Mode Selector overlay
        Gtk::Overlay _overlay{};

        // MapTools, Tool config
        Gtk::Paned _sidebar_vsplitter_L{Gtk::Orientation::ORIENTATION_VERTICAL};
        // Outliner, Property Editor
        Gtk::Paned _sidebar_vsplitter_R{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::Paned _sidebar_splitter_L{
            Gtk::Orientation::ORIENTATION_HORIZONTAL};
        Gtk::Paned _sidebar_splitter_R{
            Gtk::Orientation::ORIENTATION_HORIZONTAL};

        Glib::Property<guint> _prop_grid_size;
        Glib::RefPtr<Glib::Binding> _binding_grid_size_top,
            _binding_grid_size_front,
            _binding_grid_size_right;
        Glib::RefPtr<Glib::Binding> _binding_left_right_views_position;
        Glib::RefPtr<Glib::Binding> _binding_editor_modeselector_mode;
        Glib::RefPtr<Glib::Binding> _binding_editor_outliner_world;
        Glib::RefPtr<Glib::Binding> _binding_views_horizontal_half_position;
        Glib::RefPtr<Glib::Binding> _binding_views_vertical_half_position;
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

        static std::string _make_lua_include_path(std::string const &dir);

        void _on_grid_size_changed();
        void _on_opsearch_op_chosen(Editor::Operation const &op);

        void _sync_property_editor();

        void _run_internal_scripts();
        void _run_runtime_scripts();
        void _run_operations_scripts();
    };
}

#endif
