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

#include <glibmm/fileutils.h>
#include <gtkmm/messagedialog.h>

#include <algorithm>
#include <fstream>
#include <iostream>


#define GRID_SIZE_MIN   1U
#define GRID_SIZE_MAX   512U


Sickle::AppWin::AppWin()
:   Glib::ObjectBase{typeid(AppWin)}
,   Gtk::ApplicationWindow{}
,   editor{}
,   L{luaL_newstate()}
,   m_grid{}
,   m_viewgrid{}
,   m_maparea{editor}
,   m_drawarea_top{editor}
,   m_drawarea_front{editor}
,   m_drawarea_right{editor}
,   m_luaconsolewindow{}
,   m_luaconsole{}
,   m_hbox{}
,   m_gridsizelabel{}
,   _prop_grid_size{*this, "grid-size", 32}
,   _binding_grid_size_top{}
,   _binding_grid_size_front{}
,   _binding_grid_size_right{}
{
    if (!L)
        throw Lua::Error{"Failed to allocate new lua_State"};
    luaL_checkversion(L);
    luaL_openlibs(L);


    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);

    _binding_grid_size_top = Glib::Binding::bind_property(
        property_grid_size(),
        m_drawarea_top.property_grid_size(),
        Glib::BindingFlags::BINDING_SYNC_CREATE);
    _binding_grid_size_front = Glib::Binding::bind_property(
        property_grid_size(),
        m_drawarea_front.property_grid_size(),
        Glib::BindingFlags::BINDING_SYNC_CREATE);
    _binding_grid_size_right = Glib::Binding::bind_property(
        property_grid_size(),
        m_drawarea_right.property_grid_size(),
        Glib::BindingFlags::BINDING_SYNC_CREATE);

    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, &AppWin::_on_grid_size_changed));
    _on_grid_size_changed();

    add_events(Gdk::KEY_PRESS_MASK);

    m_drawarea_top.set_draw_angle(Sickle::MapArea2D::DrawAngle::TOP);
    m_drawarea_front.set_draw_angle(Sickle::MapArea2D::DrawAngle::FRONT);
    m_drawarea_right.set_draw_angle(Sickle::MapArea2D::DrawAngle::RIGHT);

    m_viewgrid.set_row_spacing(2);
    m_viewgrid.set_column_spacing(2);
    m_viewgrid.set_row_homogeneous(true);
    m_viewgrid.set_column_homogeneous(true);
    m_viewgrid.attach(m_maparea, 0, 0);
    m_viewgrid.attach(m_drawarea_top, 1, 0);
    m_viewgrid.attach(m_drawarea_front, 0, 1);
    m_viewgrid.attach(m_drawarea_right, 1, 1);

    m_infobar.set_show_close_button(true);
    m_infobar.signal_response().connect(
        [this](int){m_infobar.hide();});
    m_infobar.set_message_type(Gtk::MessageType::MESSAGE_INFO);
    m_infobar_label.set_text("Reloaded Lua scripts");
    auto contentarea = dynamic_cast<Gtk::Container *>(
        m_infobar.get_content_area());
    contentarea->add(m_infobar_label);
    signal_lua_reloaded().connect(
        sigc::mem_fun(m_infobar, &Gtk::InfoBar::show));

    m_hbox.pack_end(m_gridsizelabel);
    m_hbox.pack_start(m_infobar);

    m_grid.attach(m_maptools, 0, 0);
    m_grid.attach(m_viewgrid, 1, 0);
    m_grid.attach(m_hbox, 0, 1, 2);
    add(m_grid);

    // Lua console window
    m_luaconsole.property_lua_state().set_value(L);
    m_luaconsole.set_size_request(320, 240);
    m_luaconsolewindow.add(m_luaconsole);
    m_luaconsolewindow.show_all_children();
    m_luaconsolewindow.set_title(SE_CANON_NAME " - Lua Console");
    signal_lua_reloaded().connect(
        [this](){m_luaconsole.writeline("---Lua Reloaded---");});

    setup_lua_state();
    reload_scripts();

    show_all_children();
    m_infobar.hide();
}

Sickle::Editor::Map loadAnyMapFile(Glib::RefPtr<Gio::File> const &file)
{
    std::string maperror{};
    std::string rmferror{};
    try
    {
        return RMF::load(file->get_path());
    }
    catch (RMF::LoadError const &e)
    {
        rmferror = e.what();
    }
    try
    {
        return MAP::load(file->get_path());
    }
    catch (MAP::LoadError const &e)
    {
        maperror = e.what();
    }
    Gtk::MessageDialog d{
        "Failed to load " + file->get_path() + ":\n"
        + ".map: " + maperror + "\n"
        + ".rmf: " + rmferror,
        false,
        Gtk::MessageType::MESSAGE_ERROR};
    d.set_title("File Load Error");
    d.run();
    return {};
}

void Sickle::AppWin::open(Glib::RefPtr<Gio::File> const &file)
{
    if (file)
        editor.set_map(loadAnyMapFile(file));
    else
        editor.set_map({});
}

void Sickle::AppWin::save(std::string const &filename)
{
    std::ofstream out{filename};
    MAP::save(out, editor.get_map());
}

void Sickle::AppWin::show_console_window()
{
    m_luaconsolewindow.present();
}

void Sickle::AppWin::reload_scripts()
{
    auto pre = lua_gettop(L);
    for (auto const &path : _lua_script_dirs)
    {
        auto dir = Gio::File::create_for_path(path);
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

void Sickle::AppWin::set_grid_size(guint grid_size)
{
    property_grid_size() = std::clamp(grid_size, GRID_SIZE_MIN, GRID_SIZE_MAX);
}

guint Sickle::AppWin::get_grid_size()
{
    return property_grid_size().get_value();
}

Sickle::MapTools::Tool Sickle::AppWin::get_maptool()
{
    return m_maptools.property_tool().get_value();
}


void Sickle::AppWin::setup_lua_state()
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
        auto dir = Gio::File::create_for_path(path);
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
    std::string oldpath{lua_tostring(L, -1)};
    lua_pop(L, 1);

    std::stringstream paths{};
    for (auto const &dir : dirs)
    {
        paths << dir->get_path() + "/?;";
        paths << dir->get_path() + "/?.lua;";
    }
    Lua::push(L, oldpath + ";" + paths.str());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);
}


void Sickle::AppWin::_on_grid_size_changed()
{
    m_gridsizelabel.set_text(
        "Grid Size: " + std::to_string(property_grid_size().get_value()));
}
