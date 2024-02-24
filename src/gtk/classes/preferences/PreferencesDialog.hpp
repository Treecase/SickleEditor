/**
 * PreferencesDialog.hpp - Sickle Preferences dialog.
 * Copyright (C) 2024 Trevor Last
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

#ifndef SE_PREFERENCESDIALOG_HPP
#define SE_PREFERENCESDIALOG_HPP

#include "WADList.hpp"

#include <giomm/settings.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>


namespace Sickle
{
    /**
     * Preferences management window.
     *
     * Allows the user to view and modify global editor settings.
     */
    class PreferencesDialog : public Gtk::Dialog
    {
    public:
        PreferencesDialog(Gtk::Window &parent);
        virtual ~PreferencesDialog()=default;

    protected:
        virtual void on_response(int response_id) override;

    private:
        Glib::RefPtr<Gio::Settings> _settings{nullptr};

        Gtk::Grid _grid{};
        Gtk::Label _gamedef_label{"Game Definition"};
        Gtk::Entry _gamedef_entry{};
        Gtk::Label _sprite_path_label{"Sprite Root Path"};
        Gtk::Entry _sprite_path_entry{};
        WADList _wads{};

        void _apply_preferences();

        void on_gamedef_entry_icon_pressed(
            Gtk::EntryIconPosition icon_pos,
            GdkEventButton const *event);
        void on_sprite_path_entry_icon_pressed(
            Gtk::EntryIconPosition icon_pos,
            GdkEventButton const *event);
    };
}

#endif
