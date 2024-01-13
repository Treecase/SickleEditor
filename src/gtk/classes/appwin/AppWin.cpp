/**
 * AppWin.cpp - Sickle ApplicationWindow.
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

#include "AppWin.hpp"
#include "AppWin_Lua.hpp"
#include "MapArea2D_Lua.hpp"
#include "appid.hpp"
#include "version.hpp"

#include <LuaGeo.hpp>
#include <map/mapsaver.hpp>
#include <rmf/rmf.hpp>
#include <se-lua/se-lua.hpp>
#include <se-lua/function.hpp>

#include <giomm/resource.h>
#include <glibmm/fileutils.h>
#include <gtkmm/builder.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/settings.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <typeinfo>


using namespace Sickle::AppWin;


/** Convert a string to lowercase. */
std::string lowercase(std::string s)
{
    std::transform(
        s.begin(), s.end(),
        s.begin(),
        [](unsigned char ch){return std::tolower(ch);});
    return s;
}


/**
 * Thrown by 'loadAnyMapFile' if neither RMF or MAP format can load the file
 * correctly.
 */
struct GenericLoadError : public std::runtime_error
{
    std::string const rmf, map;
    GenericLoadError(std::string const &rmf, std::string const &map)
    :   std::runtime_error{rmf + ";" + map}
    ,   rmf{rmf}
    ,   map{map}
    {
    }
};


static int panic_handler(lua_State *L)
{
    std::string const errmsg{lua_tostring(L, -1)};
    throw std::runtime_error{"AppWin: Fatal Lua Error -- " + errmsg};
    return 0;
}


AppWin::AppWin()
:   Glib::ObjectBase{typeid(AppWin)}
,   Gtk::ApplicationWindow{}
,   Lua::Referenceable{}
,   L{luaL_newstate()}
,   editor{Editor::Editor::create(L)}
,   _view3d{editor}
,   _view2d_top{editor}
,   _view2d_front{editor}
,   _view2d_right{editor}
,   _maptools{editor}
,   _maptool_config{editor}
,   _opsearch{editor}
,   _prop_grid_size{*this, "grid-size", 32}
,   _binding_grid_size_top{
        Glib::Binding::bind_property(
            property_grid_size(),
            _view2d_top.property_grid_size(),
            Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _binding_grid_size_front{
        Glib::Binding::bind_property(
            property_grid_size(),
            _view2d_front.property_grid_size(),
            Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _binding_grid_size_right{
        Glib::Binding::bind_property(
            property_grid_size(),
            _view2d_right.property_grid_size(),
            Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _binding_left_right_views_position{
        Glib::Binding::bind_property(
            _left_views.property_position(),
            _right_views.property_position(),
            Glib::BindingFlags::BINDING_BIDIRECTIONAL)}
,   _binding_editor_modeselector_mode{
        Glib::Binding::bind_property(
            editor->property_mode(),
            _mode_selector.property_mode(),
            Glib::BindingFlags::BINDING_BIDIRECTIONAL)}
,   _binding_editor_outliner_world{
        Glib::Binding::bind_property(
            editor->property_map(),
            _outliner.property_world(),
            Glib::BindingFlags::BINDING_SYNC_CREATE)}
,   _binding_views_horizontal_half_position{
        Glib::Binding::bind_property(
            _sidebar_splitter_R.property_position(),
            _views.property_position(),
            Glib::BindingFlags::BINDING_DEFAULT,
            Glib::Binding::SlotTypedTransform<int, int>{
                [](int const &from, int &to) -> bool {
                    to = from / 2;
                    return true;
                }
            })}
{
    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);

    auto settings = Gtk::Settings::get_default();
    settings->property_gtk_application_prefer_dark_theme() = true;

    add_action(
        "openLuaConsole",
        sigc::mem_fun(*this, &AppWin::on_action_openLuaConsole));
    add_action(
        "openLuaDebugger",
        sigc::mem_fun(*this, &AppWin::on_action_openLuaDebugger));
    add_action(
        "reloadLua",
        sigc::mem_fun(*this, &AppWin::on_action_reloadLua));

    // TODO: integrate w/ dynamic system?
    add_action(
        "mapTools_Select",
        [this](){editor->set_maptool("Select");});
    add_action(
        "mapTools_CreateBrush",
        [this](){editor->set_maptool("Create Brush");});

    add_events(Gdk::EventMask::KEY_PRESS_MASK);

    editor->property_mode() = "brush";

    // TODO: Use Gtk::Builder to clean this up?
    _view2d_top.set_draw_angle(Sickle::MapArea2D::DrawAngle::TOP);
    _view2d_front.set_draw_angle(Sickle::MapArea2D::DrawAngle::FRONT);
    _view2d_right.set_draw_angle(Sickle::MapArea2D::DrawAngle::RIGHT);

    _left_views.add1(_view3d);
    _left_views.add2(_view2d_front);
    _left_views.set_wide_handle(true);

    _right_views.add1(_view2d_top);
    _right_views.add2(_view2d_right);
    _right_views.set_wide_handle(true);

    _views.add1(_left_views);
    _views.add2(_right_views);
    _views.set_wide_handle(true);

    // Top and bottom views should be 1/2 the height of the main view area.
    // FIXME: Known bugs
    // - Doesn't work properly on startup or maximize/unmaximize events.
    _views.signal_size_allocate().connect(
        [this](Gtk::Allocation const &a){
            _left_views.set_position(a.get_height() / 2);
            _right_views.set_position(a.get_height() / 2);
        });

    _mode_selector.add_mode("brush", "Brush");
    _mode_selector.add_mode("face", "Face");
    _mode_selector.add_mode("vertex", "Vertex");
    _mode_selector.property_mode() = "brush";

    editor->selected.signal_updated().connect(
        sigc::mem_fun(*this, &AppWin::_sync_property_editor));

    _overlay.add(_views);
    _overlay.add_overlay(_mode_selector);

    _outliner.property_editor().set_value(editor);

    _sidebar_vsplitter_L.pack1(_maptools, Gtk::AttachOptions::EXPAND);
    _sidebar_vsplitter_L.pack2(_maptool_config, Gtk::AttachOptions::EXPAND);

    _sidebar_vsplitter_R.pack1(_outliner, Gtk::AttachOptions::EXPAND);
    _sidebar_vsplitter_R.pack2(_property_editor, Gtk::AttachOptions::EXPAND);

    _sidebar_splitter_R.pack1(_overlay, Gtk::AttachOptions::EXPAND);
    _sidebar_splitter_R.pack2(_sidebar_vsplitter_R, Gtk::AttachOptions::SHRINK);

    _sidebar_splitter_L.pack1(_sidebar_vsplitter_L, Gtk::AttachOptions::SHRINK);
    _sidebar_splitter_L.pack2(_sidebar_splitter_R, Gtk::AttachOptions::EXPAND);

    _luainfobar.set_show_close_button(true);
    _luainfobar.set_message_type(Gtk::MessageType::MESSAGE_INFO);
    _luainfobarlabel.set_text("Reloaded Lua scripts");
    auto contentarea = dynamic_cast<Gtk::Container *>(
        _luainfobar.get_content_area());
    contentarea->add(_luainfobarlabel);

    _inforegion.pack_end(_gridsizelabel);
    _inforegion.pack_start(_luainfobar);

    _maptool_config.signal_confirmed().connect(
        sigc::mem_fun(*this, &AppWin::on_maptoolconfig_confirmed));

    _basegrid.attach(_sidebar_splitter_L, 0, 0);
    _basegrid.attach(_inforegion, 0, 1, 2);
    add(_basegrid);

    _lua_console_window.add(_lua_console);
    _lua_console_window.set_title(SE_CANON_NAME " - Lua Console");
    _lua_console_window.show_all_children();

    _lua_debugger_window.set_title(SE_CANON_NAME " - Lua Debugger");

    _luainfobar.signal_response().connect([this](int){_luainfobar.hide();});
    signal_lua_reloaded().connect(
        sigc::mem_fun(_luainfobar, &Gtk::InfoBar::show));
    signal_lua_reloaded().connect(
        [this](){
            lua_getglobal(L, "print");
            Lua::pcallT(L, 0, "---Lua Reloaded---");
        });
    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, &AppWin::_on_grid_size_changed));

    _opsearch.set_transient_for(*this);
    _opsearch.signal_operation_chosen().connect(
        sigc::mem_fun(*this, &AppWin::_on_opsearch_op_chosen));

    _on_grid_size_changed();

    if (!L)
        throw Lua::Error{"Failed to allocate new lua_State"};
    luaL_checkversion(L);

    setup_lua_state();
    reload_scripts();

    show_all_children();
    _luainfobar.hide();
}


Glib::RefPtr<Sickle::Editor::World>
loadAnyMapFile(Glib::RefPtr<Gio::File> const &file)
{
    auto const path = file->get_path();
    if (lowercase(path).rfind(".rmf") != std::string::npos)
        return Sickle::Editor::World::create(RMF::load(path));

    else if (lowercase(path).rfind(".map") != std::string::npos)
        return Sickle::Editor::World::create(MAP::load(path));

    std::string rmferror{};
    std::string maperror{};
    try {
        return Sickle::Editor::World::create(RMF::load(file->get_path()));
    }
    catch (RMF::LoadError const &e) {
        rmferror = e.what();
    }
    try {
        return Sickle::Editor::World::create(MAP::load(file->get_path()));
    }
    catch (MAP::LoadError const &e) {
        maperror = e.what();
    }
    throw GenericLoadError{rmferror, maperror};
}


void AppWin::open(Glib::RefPtr<Gio::File> const &file)
{
    if (file)
    {
        std::string errmsg{};
        try {
            editor->set_map(loadAnyMapFile(file));
            return;
        }
        catch (RMF::LoadError const &e) {
            errmsg = ".rmf: " + std::string{e.what()};
        }
        catch (MAP::LoadError const &e) {
            errmsg = ".map: " + std::string{e.what()};
        }
        catch (GenericLoadError const &e) {
            errmsg = ".rmf: " + e.rmf + "\n" + ".map: " + e.map;
        }
        Gtk::MessageDialog d{
            "Failed to load " + file->get_path() + ":\n" + errmsg,
            false,
            Gtk::MessageType::MESSAGE_ERROR};
        d.set_title("File Load Error");
        d.run();
    }
    else
        editor->set_map(Editor::World::create());
}


void AppWin::save(std::string const &filename)
{
    std::ofstream out{filename};
    MAP::save(out, *editor->get_map().get());
}


void AppWin::show_console_window()
{
    _lua_console_window.present();
}


void AppWin::show_debugger_window()
{
    _lua_debugger_window.present();
}


void AppWin::reload_scripts()
{
    // TODO: instead of trying to reset the state, just create a new one and
    // redo everything.
    auto const start_top = lua_gettop(L);

    _run_internal_scripts();
    _run_runtime_scripts();
    _run_operations_scripts();

    lua_pop(L, lua_gettop(L) - start_top);
    signal_lua_reloaded().emit();
}


void AppWin::search_operations()
{
    _opsearch.present();
}


void AppWin::add_maptool(Editor::MapTool const &maptool)
{
    editor->add_maptool(maptool);
}


void AppWin::set_grid_size(guint grid_size)
{
    property_grid_size() = std::clamp(grid_size, GRID_SIZE_MIN, GRID_SIZE_MAX);
}


guint AppWin::get_grid_size()
{
    return property_grid_size().get_value();
}



void AppWin::setup_lua_state()
{
    luaL_openlibs(L);
    lua_atpanic(L, panic_handler);

    luaL_requiref(L, "function", Lua::luaopen_function, 0);
    luaL_requiref(L, "appwin", luaopen_appwin, 1);
    luaL_requiref(L, "geo", luaopen_geo, 1);
    lua_pop(L, 3);

    Lua::set_msgh(L, [this](lua_State *L){
        int const initial = lua_gettop(L);
        _lua_debugger_window.set_pause(true);
        _lua_debugger_window.update();
        assert(initial == lua_gettop(L));
        return 1;
    });

    _lua_console.property_lua_state().set_value(L);
    _lua_debugger_window.property_lua_state().set_value(L);

    Lua::push(L, this);
    lua_setglobal(L, "gAppWin");
}


void AppWin::on_maptoolconfig_confirmed()
{
    if (_maptool_config.has_operation())
    {
        auto const op = _maptool_config.get_operation();
        op.execute(editor, _maptool_config.get_arguments());
        _maptool_config.clear_operation();
    }
}


void AppWin::on_action_openLuaConsole()
{
    show_console_window();
}


void AppWin::on_action_openLuaDebugger()
{
    show_debugger_window();
}


void AppWin::on_action_reloadLua()
{
    reload_scripts();
}


bool AppWin::on_key_press_event(GdkEventKey *event)
{
    switch (event->keyval)
    {
    // TODO: Use a proper keybind mechanism.
    case GDK_KEY_space:{
        auto const focus = get_focus();
        if (focus == &_view2d_front
            || focus == &_view2d_right
            || focus == &_view2d_top
            || focus == &_view3d)
        {
            search_operations();
            return true;
        }
        }/* fallthrough */

    default:
        return Gtk::ApplicationWindow::on_key_press_event(event);
    }
}



std::string AppWin::_make_lua_include_path(std::string const &path)
{
    return path + "/?.lua;" + path + "/?";
}


void AppWin::_on_grid_size_changed()
{
    _gridsizelabel.set_text(
        "Grid Size: " + std::to_string(property_grid_size().get_value()));
}


void AppWin::_on_opsearch_op_chosen(Editor::Operation const &op)
{
    _maptool_config.set_operation(op);
}


void AppWin::_sync_property_editor()
{
    auto const entity = editor->selected.get_latest_of_type<Editor::Entity>();
    _property_editor.set_entity(entity);
}


void AppWin::_run_internal_scripts()
{
    int const start_top = lua_gettop(L);
    for (auto const &path : _internal_scripts)
    {
        auto const &res = Gio::Resource::lookup_data_global(
            SE_GRESOURCE_PREFIX + path);
        gsize _size;
        try {
            Lua::checkerror(L,
                luaL_dostring(
                    L,
                    static_cast<char const *>(res->get_data(_size))));
        }
        catch (Lua::Error const &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    lua_pop(L, lua_gettop(L) - start_top);
}


void AppWin::_run_runtime_scripts()
{
    int const start_top = lua_gettop(L);

    // Save old include paths.
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    std::string const oldPATH{lua_tostring(L, -1)};
    lua_pop(L, 1);

    // Load and execute runtime scripts.
    int const start_top1 = lua_gettop(L);
    for (auto const dir_path : _lua_script_dirs)
    {
        auto const newPATH = oldPATH + ";" + _make_lua_include_path(dir_path);
        Lua::push(L, newPATH);
        lua_setfield(L, -2, "path");

        auto const script_path = dir_path + "/main.lua";
        auto const r = luaL_loadfile(L, script_path.c_str());
        switch (r)
        {
        case LUA_OK:
            try {
                Lua::checkerror(L, Lua::pcall(L, 0, LUA_MULTRET));
            }
            catch (Lua::Error const &e) {
                std::cout << e.what() << std::endl;
            }
            break;
        default:
            auto const errmsg = lua_tostring(L, -1);
            std::cout << errmsg << std::endl;
            lua_pop(L, 1);
            break;
        }
        lua_pop(L, lua_gettop(L) - start_top1);
    }

    // Reset include paths.
    Lua::push(L, oldPATH);
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);

    assert(lua_gettop(L) == start_top);
    lua_pop(L, lua_gettop(L) - start_top);
}


void AppWin::_run_operations_scripts()
{
    int const start_top = lua_gettop(L);

    // Save old include paths.
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    std::string const oldPATH{lua_tostring(L, -1)};
    lua_pop(L, 1);

    // Load and execute operation scripts.
    int const start_top1 = lua_gettop(L);
    for (auto const dir_path : _operation_script_dirs)
    {
        auto const newPATH = oldPATH + ";" + _make_lua_include_path(dir_path);
        Lua::push(L, newPATH);
        lua_setfield(L, -2, "path");

        auto const dir = Gio::File::create_for_path(dir_path);
        Glib::RefPtr<Gio::FileEnumerator> e{nullptr};
        try {
            e = dir->enumerate_children();
        }
        catch (Gio::Error const &e) {
            std::cerr << e.what() << std::endl;
            continue;
        }

        for (auto file = e->next_file(); file; file = e->next_file())
        {
            try {
                editor->oploader->add_source_from_file(
                    dir_path + "/" + file->get_name());
            }
            catch (Lua::Error const &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        lua_pop(L, lua_gettop(L) - start_top1);
    }

    // Reset include paths.
    Lua::push(L, oldPATH);
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);

    assert(lua_gettop(L) == start_top);
    lua_pop(L, lua_gettop(L) - start_top);
}
