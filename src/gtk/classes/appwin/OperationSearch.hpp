/**
 * OperationSearch.cpp - Operation Search popup.
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

#ifndef SE_APPWIN_OPERATIONSEARCH_HPP
#define SE_APPWIN_OPERATIONSEARCH_HPP

#include <editor/operations/Operation.hpp>

#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <gtkmm/liststore.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>

#include <vector>


namespace Sickle::AppWin
{
    /** Popup for searching Operations. */
    class OperationSearch : public Glib::ObjectBase
    {
        struct OperationModel : public Gtk::TreeModelColumnRecord
        {
            Gtk::TreeModelColumn<Editor::Operation const *> col1{};
            Gtk::TreeModelColumn<Glib::ustring> col2{};
            OperationModel();
        } _model{};

        sigc::signal<void(Editor::Operation)> _sig_operation_chosen{};

        Editor::EditorRef _editor;
        Gtk::Window *window{nullptr};
        Gtk::SearchEntry *searchbar{nullptr};
        Gtk::TreeView *treeview{nullptr};
        Glib::RefPtr<Gtk::ListStore> operations{nullptr};
        Glib::RefPtr<Gtk::TreeModelFilter> filtered{nullptr};
        Glib::RefPtr<Gtk::TreeModelSort> sorted{nullptr};
        Gtk::CellRendererText cellrenderer{};

        static size_t similarity(
            Glib::ustring const &a,
            Glib::ustring const &b);

        void on_search_changed();
        void on_row_activated(
            Gtk::TreeModel::Path const &path,
            Gtk::TreeView::Column *column);
        void on_editor_operation_added(std::string const &id);

        bool filter_visible_func(Gtk::TreeModel::iterator const &iter);
        int operations_sort_func(
            Gtk::TreeModel::iterator const &a,
            Gtk::TreeModel::iterator const &b);

        void _add_row(Editor::Operation const &op);

    public:
        OperationSearch(Editor::EditorRef editor);
        virtual ~OperationSearch();

        void set_transient_for(Gtk::Window &parent);
        void present();

        auto &signal_operation_chosen() {return _sig_operation_chosen;}
    };
}

#endif
