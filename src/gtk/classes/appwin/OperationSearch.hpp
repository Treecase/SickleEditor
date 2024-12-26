/**
 * OperationSearch.cpp - Operation Search popup.
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

#ifndef SE_APPWIN_OPERATIONSEARCH_HPP
#define SE_APPWIN_OPERATIONSEARCH_HPP

#include <editor/core/Editor.hpp>
#include <editor/operations/Operation.hpp>

#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>

#include <memory>
#include <vector>

namespace Sickle::AppWin
{
    /**
     * Popup for searching Operations.
     *
     * Allows the user to search for and select an Operation from the list of
     * all operations.
     */
    class OperationSearch : public Gtk::Window
    {
    public:
        /**
         * Construct a new OperationSearch.
         *
         * @param editor Reference to the editor managing the operations.
         * @return A pointer to an OperationSearch object.
         */
        static OperationSearch *create(Editor::EditorRef const &editor);

        /// @warning Only to be used by main to initialize the dummy instance.
        OperationSearch();
        OperationSearch(
            BaseObjectType *cobject,
            Glib::RefPtr<Gtk::Builder> const &builder,
            Editor::EditorRef const &editor);
        virtual ~OperationSearch();

        /** Emitted when an operation is chosen. */
        auto &signal_operation_chosen() { return _sig_operation_chosen; }

    protected:
        virtual bool on_focus_out_event(GdkEventFocus *gdk_event) override;
        virtual void on_hide() override;
        virtual bool on_key_press_event(GdkEventKey *key_event) override;

    private:
        struct Columns : public Gtk::TreeModelColumnRecord
        {
            Gtk::TreeModelColumn<std::shared_ptr<Editor::Operation>>
                operation{};
            Gtk::TreeModelColumn<Glib::ustring> name{};

            Columns()
            {
                add(operation);
                add(name);
            }
        };

        sigc::signal<void(Editor::Operation)> _sig_operation_chosen{};

        Editor::EditorRef _editor{nullptr};
        Glib::RefPtr<Gtk::ListStore> _operations{nullptr};
        Glib::RefPtr<Gtk::TreeModelFilter> _filtered{nullptr};
        Glib::RefPtr<Gtk::TreeModelSort> _sorted{nullptr};

        Gtk::SearchEntry *_searchbar{nullptr};
        Gtk::TreeView *_treeview{nullptr};

        static Columns &_columns();
        static size_t _similarity(
            Glib::ustring const &a,
            Glib::ustring const &b);

        void _add_row(Editor::Operation const &op);
        bool _filter_visible_func(Gtk::TreeModel::iterator const &iter) const;
        int _operations_sort_func(
            Gtk::TreeModel::iterator const &a,
            Gtk::TreeModel::iterator const &b) const;

        void _on_editor_operation_added(std::string const &id);
        void _on_row_activated(
            Gtk::TreeModel::Path const &path,
            Gtk::TreeView::Column *column);
        void _on_search_changed();
    };
} // namespace Sickle::AppWin

#endif
