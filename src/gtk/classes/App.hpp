/**
 * App.hpp - Sickle app.
 * Copyright (C) 2022 Trevor Last
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
#include "fgd/fgd.hpp"

#include <gtkmm/application.h>
#include <giomm/settings.h>


namespace Sickle
{
    class App : public Gtk::Application
    {
    public:
        static Glib::RefPtr<App> create();

        auto property_fgd_path() {return _prop_fgd_path.get_proxy();}

    protected:
        Glib::RefPtr<Gio::Settings> m_settings;

        App();

        // Signals
        void on_startup() override;
        void on_activate() override;
        void on_open(Gio::Application::type_vec_files const &files, Glib::ustring const &hint) override;

        // Actions
        // Menu Actions
        void on_action_new();
        void on_action_open();
        void on_action_exit();
        void on_action_setGameDef();
        void on_action_about();

    private:
        FGD::GameDef _game_definition;

        AppWin *_create_appwindow();

        // Properties
        Glib::Property<Glib::ustring> _prop_fgd_path;

        // Signal Handlers
        void _on_hide_window(Gtk::Window *window);
        void _on_fgd_path_changed();
    };
}

#endif
