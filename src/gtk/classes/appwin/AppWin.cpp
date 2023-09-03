/**
 * AppWin.cpp - Sickle ApplicationWindow.
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

#include "AppWin.hpp"
#include "AppWin_Lua.hpp"
#include "MapArea2D_Lua.hpp"
#include "appid.hpp"
#include "version.hpp"

#include <LuaGeo.hpp>
#include <map/mapsaver.hpp>
#include <rmf/rmf.hpp>

#include <giomm/resource.h>
#include <glibmm/fileutils.h>
#include <gtkmm/builder.h>
#include <gtkmm/messagedialog.h>

#include <algorithm>
#include <fstream>
#include <iostream>


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
{
    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);

    add_action(
        "openLuaConsole",
        sigc::mem_fun(*this, &AppWin::on_action_openLuaConsole));
    add_action(
        "reloadLua",
        sigc::mem_fun(*this, &AppWin::on_action_reloadLua));

    // FIXME: temp
    for (auto const &tool : _predefined_maptools)
        editor->add_maptool(tool);

    // TODO: integrate w/ dynamic system?
    add_action(
        "mapTools_Select",
        [this](){editor->set_maptool("Select");});
    add_action(
        "mapTools_CreateBrush",
        [this](){editor->set_maptool("Create Brush");});

    add_events(Gdk::EventMask::KEY_PRESS_MASK);


    // TODO: Use Gtk::Builder to clean this up?
    _view2d_top.set_draw_angle(Sickle::MapArea2D::DrawAngle::TOP);
    _view2d_front.set_draw_angle(Sickle::MapArea2D::DrawAngle::FRONT);
    _view2d_right.set_draw_angle(Sickle::MapArea2D::DrawAngle::RIGHT);

    _viewsgrid.set_row_spacing(2);
    _viewsgrid.set_column_spacing(2);
    _viewsgrid.set_row_homogeneous(true);
    _viewsgrid.set_column_homogeneous(true);
    _viewsgrid.attach(_view3d, 0, 0);
    _viewsgrid.attach(_view2d_top, 1, 0);
    _viewsgrid.attach(_view2d_front, 0, 1);
    _viewsgrid.attach(_view2d_right, 1, 1);

    _luainfobar.set_show_close_button(true);
    _luainfobar.set_message_type(Gtk::MessageType::MESSAGE_INFO);
    _luainfobarlabel.set_text("Reloaded Lua scripts");
    auto contentarea = dynamic_cast<Gtk::Container *>(
        _luainfobar.get_content_area());
    contentarea->add(_luainfobarlabel);

    _inforegion.pack_end(_gridsizelabel);
    _inforegion.pack_start(_luainfobar);

    _basegrid.attach(_maptools, 0, 0);
    _basegrid.attach(_viewsgrid, 1, 0);
    _basegrid.attach(_inforegion, 0, 1, 2);
    add(_basegrid);

    _luaconsole.set_size_request(320, 240);
    _luaconsolewindow.add(_luaconsole);
    _luaconsolewindow.show_all_children();
    _luaconsolewindow.set_title(SE_CANON_NAME " - Lua Console");

    _luainfobar.signal_response().connect([this](int){_luainfobar.hide();});
    signal_lua_reloaded().connect(
        sigc::mem_fun(_luainfobar, &Gtk::InfoBar::show));
    signal_lua_reloaded().connect(
        [this](){_luaconsole.writeline("---Lua Reloaded---");});
    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, &AppWin::_on_grid_size_changed));

    _opsearch.set_transient_for(*this);
    _opsearch.signal_operation_chosen().connect(
        sigc::mem_fun(*this, &AppWin::_on_opsearch_op_chosen));

    _on_grid_size_changed();

    _setup_operations();

    if (!L)
        throw Lua::Error{"Failed to allocate new lua_State"};
    luaL_checkversion(L);
    luaL_openlibs(L);

    lua_atpanic(L, panic_handler);

    setup_lua_state();
    reload_scripts();

    _luaconsole.property_lua_state().set_value(L);

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
    _luaconsolewindow.present();
}


void AppWin::reload_scripts()
{
    auto const pre = lua_gettop(L);
    for (auto const &path : _lua_script_dirs)
    {
        auto const dir = Gio::File::create_for_path(path);
        if (!dir->query_exists())
            continue;
        auto enumeration = dir->enumerate_children();
        for (
            auto file = enumeration->next_file();
            file;
            file = enumeration->next_file())
        {
            auto const &filepath = dir->get_path() + "/" + file->get_name();
            if (Glib::file_test(filepath, Glib::FileTest::FILE_TEST_IS_DIR))
                continue;
            Lua::checkerror(L, luaL_dofile(L, filepath.c_str()));
        }
    }
    lua_pop(L, lua_gettop(L) - pre);
    signal_lua_reloaded().emit();
}


void AppWin::search_operations()
{
    _opsearch.present();
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
    luaL_requiref(L, "appwin", luaopen_appwin, 1);
    luaL_requiref(L, "geo", luaopen_geo, 1);
    lua_pop(L, 2);

    Lua::push(L, this);
    lua_setglobal(L, "gAppWin");

    // Run internal GResource scripts.
    for (auto const &path : _internal_scripts)
    {
        auto const &res = Gio::Resource::lookup_data_global(
            SE_GRESOURCE_PREFIX + path);
        gsize _size;
        Lua::checkerror(L,
            luaL_dostring(L, static_cast<char const *>(res->get_data(_size))));
    }

    // Get Lua scripts directory(s).
    std::vector<Glib::RefPtr<Gio::File>> dirs{};
    for (auto const &path : _lua_script_dirs)
    {
        auto const dir = Gio::File::create_for_path(path);
        if (dir->query_exists())
            dirs.push_back(dir);
    }
    if (dirs.empty())
    {
        Gtk::MessageDialog d{
            "Failed to load Lua scripts!",
            false,
            Gtk::MessageType::MESSAGE_WARNING};
        d.set_title("Warning");
        d.run();
        std::cerr << "WARNING: Failed to load Lua scripts!\n";
        return;
    }

    // Add script dirs to Lua path.
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    std::string const oldpath{lua_tostring(L, -1)};
    lua_pop(L, 1);

    std::stringstream paths{};
    for (auto const &dir : dirs)
    {
        paths << dir->get_path() + "/?.lua;";
        paths << dir->get_path() + "/?;";
    }
    Lua::push(L, oldpath + ";" + paths.str());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);
}


void AppWin::on_action_openLuaConsole()
{
    show_console_window();
}


void AppWin::on_action_reloadLua()
{
    reload_scripts();
}


bool AppWin::on_key_press_event(GdkEventKey *event)
{
    switch (event->keyval)
    {
    case GDK_KEY_space:
        search_operations();
        return true;

    default:
        return Gtk::ApplicationWindow::on_key_press_event(event);
    }
}



void AppWin::_on_grid_size_changed()
{
    _gridsizelabel.set_text(
        "Grid Size: " + std::to_string(property_grid_size().get_value()));
}


void AppWin::_on_opsearch_op_chosen(Editor::Operation const &op)
{
    op.execute(editor);
}


void AppWin::_setup_operations()
{
    for (auto const &path : _internal_scripts_operations)
    {
        auto const &b = Gio::Resource::lookup_data_global(
            SE_GRESOURCE_PREFIX + path);
        gsize size = 0;
        std::string const src{static_cast<char const *>(b->get_data(size))};
        editor->oploader->add_source(src);
    }
}
