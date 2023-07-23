/**
 * WADDialog.hpp - Sickle "Texture WADs" dialog.
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

#ifndef SE_WADDIALOG_HPP
#define SE_WADDIALOG_HPP

#include <giomm/settings.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>


namespace Sickle
{
    class WADModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> col_path{};
        WADModelColumns();
    };

    class WADDialog : public Gtk::Dialog
    {
    public:
        WADDialog(Gtk::Window &parent);

    protected:
        void on_response(int response) override;
        void on_add_path_clicked();
        void on_remove_path_clicked();

    private:
        Glib::RefPtr<Gio::Settings> _settings{};
        Glib::RefPtr<Gtk::ListStore> _paths{};
        WADModelColumns _columns{};
        Gtk::TreeView _pathview{};
        Gtk::Box _button_box{};
        Gtk::Button _add_path{"Add"};
        Gtk::Button _remove_path{"Remove"};

        void _on_paths_updated(Glib::ustring);
    };
}

#endif
