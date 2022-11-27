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

#ifndef _SE_APP_HPP
#define _SE_APP_HPP

#include "AppWin.hpp"
#include "../../../fgd/fgd.hpp"

#include <gtkmm.h>


namespace Sickle
{
    class App : public Gtk::Application
    {
    public:
        static Glib::RefPtr<App> create();

    protected:
        App();

        // Signals
        void on_startup() override;
        void on_activate() override;
        void on_open(Gio::Application::type_vec_files const &files, Glib::ustring const &hint) override;

        Glib::ustring game_definition_path;
        FGD::FGD game_definition;

    private:
        AppWin *create_appwindow();
        // Actions
        void on_hide_window(Gtk::Window *window);
        void on_dialog_response(int response, Gtk::Dialog *dialog);
        // > Menu actions
        void on_action_new();
        void on_action_open();
        void on_action_exit();
        void on_action_about();
    };
}

#endif
