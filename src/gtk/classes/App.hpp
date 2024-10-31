/**
 * App.hpp - Sickle app.
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

#ifndef SE_APP_HPP
#define SE_APP_HPP

#include "AppWin.hpp"
#include "preferences/PreferencesDialog.hpp"

#include <files/fgd/fgd.hpp>

#include <giomm/settings.h>
#include <gtkmm/application.h>

namespace Sickle
{
    class App : public Gtk::Application
    {
    public:
        static Glib::RefPtr<App> create();

        auto property_fgd_path() { return _prop_fgd_path.get_proxy(); }

        auto property_game_root_path()
        {
            return _prop_game_root_path.get_proxy();
        }

        auto property_sprite_root_path()
        {
            return _prop_sprite_root_path.get_proxy();
        }

        auto property_wad_paths() { return _prop_wad_paths.get_proxy(); }

    protected:
        App();

        // Signals
        void on_startup() override;
        void on_activate() override;
        void on_open(
            Gio::Application::type_vec_files const &files,
            Glib::ustring const &hint) override;

        // Actions
        // Menu Actions
        // File
        void on_action_new();
        void on_action_open();
        void on_action_save();
        void on_action_exit();
        // Edit
        void on_action_preferences();
        // Help
        void on_action_about();

    private:
        Glib::RefPtr<Gio::Settings> _settings;
        FGD::GameDef _game_definition;

        Glib::Property<Glib::ustring> _prop_fgd_path;
        Glib::Property<Glib::ustring> _prop_game_root_path;
        Glib::Property<Glib::ustring> _prop_sprite_root_path;
        Glib::Property<std::vector<Glib::ustring>> _prop_wad_paths;

        PreferencesDialog *_open_preferences();

        AppWin::AppWin *_create_appwindow();
        void _sync_wadpaths();

        // Signal Handlers
        void _on_hide_window(Gtk::Window *window);

        void _on_fgd_path_changed();
        void _on_game_root_path_changed();
        void _on_sprite_root_path_changed();
        void _on_wad_paths_changed();
    };
} // namespace Sickle

#endif
