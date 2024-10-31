/**
 * WADList.hpp - Sickle "Texture WADs" editor.
 * Copyright (C) 2023-2024 Trevor Last
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

#ifndef SE_PREFERENCES_WADLIST_HPP
#define SE_PREFERENCES_WADLIST_HPP

#include <giomm/settings.h>
#include <glibmm/property.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

#include <set>

namespace Sickle
{
    /**
     * Widget which allows the management of WADs used by the editor to load
     * textures.
     */
    class WADList : public Gtk::Frame
    {
    public:
        WADList();

        /** The set of WAD paths added to the editor. */
        auto property_wad_paths() { return _prop_wad_paths.get_proxy(); }

        /** The set of WAD paths added to the editor. */
        auto property_wad_paths() const { return _prop_wad_paths.get_proxy(); }

    protected:
        void on_add_path_clicked();
        void on_remove_path_clicked();

    private:
        class Columns : public Gtk::TreeModelColumnRecord
        {
        public:
            Gtk::TreeModelColumn<Glib::ustring> col_path{};
            Columns();
        };

        Glib::Property<std::set<Glib::ustring>> _prop_wad_paths;

        Columns _columns{};
        Glib::RefPtr<Gtk::ListStore> _paths{nullptr};

        Gtk::Box _content{Gtk::Orientation::ORIENTATION_VERTICAL};
        Gtk::TreeView _pathview;
        Gtk::Box _button_box{Gtk::Orientation::ORIENTATION_HORIZONTAL};
        Gtk::Button _add_path{"Add"};
        Gtk::Button _remove_path{"Remove"};

        void _on_wad_paths_changed();
    };
} // namespace Sickle

#endif
