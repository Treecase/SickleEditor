/**
 * Outliner.hpp - The Sickle object outliner.
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

#ifndef SE_APPWIN_OUTLINER_HPP
#define SE_APPWIN_OUTLINER_HPP

#include <core/Editor.hpp>
#include <world/World.hpp>

#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include <memory>


namespace Sickle::AppWin
{
    /**
     * A tree view of all objects in the world.
     */
    class Outliner : public Gtk::Bin
    {
    public:
        Outliner();

        /** The current World to view children of. */
        auto property_world() {return _prop_world.get_proxy();}
        /** The current World to view children of. */
        auto property_world() const {return _prop_world.get_proxy();}

        auto property_editor() {return _prop_editor.get_proxy();}
        auto property_editor() const {return _prop_editor.get_proxy();}

    protected:
        void on_button_press_event_notify(GdkEventButton *event);

        void on_object_is_selected_changed(
            Gtk::TreeModel::iterator const &iter);
        void on_selection_changed();
        void on_world_changed();

    private:
        // Stored alongside an object in the TreeModel to automatically
        // disconnect signals when the object is removed from the tree.
        struct Signals
        {
            sigc::connection selected;
            sigc::connection added;
            sigc::connection removed;

            virtual ~Signals()
            {
                selected.disconnect();
                added.disconnect();
                removed.disconnect();
            }
        };

        struct Columns : Gtk::TreeModelColumnRecord
        {
            Columns()
            :   Gtk::TreeModelColumnRecord{}
            {
                add(text);
                add(icon);
                add(ptr);
                add(signals);
            }
            Gtk::TreeModelColumn<Glib::ustring> text;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> icon;
            Gtk::TreeModelColumn<
                Glib::RefPtr<Sickle::Editor::EditorObject>> ptr;
            Gtk::TreeModelColumn<std::shared_ptr<Signals>> signals;
        };

        Glib::Property<Editor::EditorRef> _prop_editor;
        Glib::Property<Glib::RefPtr<Editor::World>> _prop_world;

        Gtk::ScrolledWindow _scrolled{};
        Columns _tree_columns{};
        Glib::RefPtr<Gtk::TreeStore> _tree_store{
            Gtk::TreeStore::create(_tree_columns)};
        Gtk::TreeView _tree_view{};
        Gtk::Menu _popup{};

        void _add_object(
            Glib::RefPtr<Sickle::Editor::EditorObject> obj,
            Gtk::TreeModel::iterator const &iter);

        void _remove_object(Gtk::TreeModel::iterator const &iter);

        void _run_operation(std::string const &operation_name);
    };
}

#endif
