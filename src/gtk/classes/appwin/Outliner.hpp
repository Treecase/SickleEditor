/**
 * Outliner.hpp - The Sickle object outliner.
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

#ifndef SE_APPWIN_OUTLINER_HPP
#define SE_APPWIN_OUTLINER_HPP

#include <world/World.hpp>

#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/scrolledwindow.h>


namespace Sickle::AppWin
{
    /**
     * A tree view of all objects in the map.
     */
    class Outliner : public Gtk::Bin
    {
    public:
        Outliner();

        auto property_world() {return _prop_world.get_proxy();}

    protected:
        void on_object_is_selected_changed(
            Glib::RefPtr<Sickle::Editor::EditorObject> const &obj);
        void on_selection_changed();
        void on_world_changed();

    private:
        struct Columns : Gtk::TreeModelColumnRecord
        {
            Columns()
            :   Gtk::TreeModelColumnRecord{}
            {
                add(text);
                add(icon);
                add(ptr);
            }
            Gtk::TreeModelColumn<Glib::ustring> text;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> icon;
            Gtk::TreeModelColumn<
                Glib::RefPtr<Sickle::Editor::EditorObject>> ptr;
        };

        Glib::Property<Glib::RefPtr<Editor::World>> _prop_world;

        Gtk::ScrolledWindow _scrolled{};
        Columns _tree_columns{};
        Glib::RefPtr<Gtk::TreeStore> _tree_store{
            Gtk::TreeStore::create(_tree_columns)};
        Gtk::TreeView _tree_view{};

        void _add_object(
            Glib::RefPtr<Sickle::Editor::EditorObject> obj,
            Gtk::TreeRow const &parent_row);
    };
}

#endif
