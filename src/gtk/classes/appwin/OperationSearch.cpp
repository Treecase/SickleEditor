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

#include "AppWin.hpp"

#include <gtkmm/builder.h>


using namespace Sickle::AppWin;


/* ===[ OperationSearch ]=== */
size_t OperationSearch::similarity(
    Glib::ustring const &a, Glib::ustring const &b)
{
    for (size_t i = 0; i < a.size() && i < b.size(); ++i)
    {
        if (a.at(i) != b.at(i))
            return i;
    }
    return b.size();
}


OperationSearch::OperationSearch(Glib::RefPtr<Editor::Editor> editor)
:   _editor{editor}
{
    auto const builder = Gtk::Builder::create_from_resource(
        SE_GRESOURCE_PREFIX "gtk/OperationSearch.glade");

    builder->get_widget("OperationSearch", window);
    builder->get_widget("SearchBar", searchbar);
    builder->get_widget("OperationView", treeview);
    operations = Gtk::ListStore::create(_model);
    filtered = Gtk::TreeModelFilter::create(operations);
    sorted = Gtk::TreeModelSort::create(filtered);

    searchbar->signal_search_changed().connect(
        sigc::mem_fun(*this, &OperationSearch::on_search_changed));

    treeview->set_model(sorted);
    treeview->append_column("Operation", _model.col2);
    treeview->set_headers_visible(false);
    treeview->signal_row_activated().connect(
        sigc::mem_fun(*this, &OperationSearch::on_row_activated));

    filtered->set_visible_func(
        sigc::mem_fun(*this, &OperationSearch::filter_visible_func));

    sorted->set_default_sort_func(
        sigc::mem_fun(*this, &OperationSearch::operations_sort_func));

    _editor->oploader->signal_operation_added().connect(
        sigc::mem_fun(*this, &OperationSearch::on_editor_operation_added));
    _editor->property_mode().signal_changed().connect(
        [this](){filtered->refilter();});

    auto const &ops = _editor->oploader->get_operations();
    for (auto const &operation : ops)
        _add_row(operation);
}


OperationSearch::~OperationSearch()
{
    auto const free_operations = [this](Gtk::TreeIter const &it){
        delete (*it)[_model.col1];
        return false;
    };
    operations->foreach_iter(free_operations);
    delete window;
}


void OperationSearch::on_search_changed()
{
    filtered->refilter();
}


void OperationSearch::on_row_activated(
    Gtk::TreeModel::Path const &path,
    Gtk::TreeView::Column *column)
{
    auto const path2 = sorted->convert_path_to_child_path(path);
    auto const path3 = filtered->convert_path_to_child_path(path2);
    auto const iter = operations->get_iter(path3);

    Editor::Operation const *const v1 = (*iter)[_model.col1];
    window->hide();
    searchbar->set_text("");

    _sig_operation_chosen.emit(*v1);
}


void OperationSearch::on_editor_operation_added(std::string const &id)
{
    _add_row(_editor->oploader->get_operation(id));
}



bool OperationSearch::filter_visible_func(
    Gtk::TreeModelFilter::iterator const &iter)
{
    if (!iter)
        return false;

    Editor::Operation const *const op = (*iter)[_model.col1];
    if (op == nullptr)
        return false;

    auto const query = searchbar->get_text().lowercase();
    auto const name = Glib::ustring{op->name}.lowercase();
    bool const name_matches = similarity(name, query) >= query.size();

    auto const mode = _editor->property_mode().get_value();
    bool const correct_mode = (op->mode == mode);

    return name_matches && correct_mode;
}


int OperationSearch::operations_sort_func(
    Gtk::TreeModel::iterator const &a,
    Gtk::TreeModel::iterator const &b)
{
    if (!a) return -1;
    if (!b) return 1;

    Editor::Operation const *const aptr{(*a)[_model.col1]};
    Editor::Operation const *const bptr{(*b)[_model.col1]};

    auto const au = Glib::ustring{aptr->name}.lowercase();
    auto const bu = Glib::ustring{bptr->name}.lowercase();

    auto const query = searchbar->get_text().lowercase();
    auto const sim_a = similarity(au, query);
    auto const sim_b = similarity(bu, query);

    if (sim_a < sim_b)
        return 1;
    else if (sim_a == sim_b)
        return au.compare(bu);
    else
        return -1;
}


void OperationSearch::_add_row(Editor::Operation const &op)
{
    auto iter = operations->append();
    (*iter)[_model.col1] = new Editor::Operation{op};
    (*iter)[_model.col2] = op.module_name + "." + op.name;
}



/* ===[ OperationModel ]=== */
OperationSearch::OperationModel::OperationModel()
{
    add(col1);
    add(col2);
}



void OperationSearch::set_transient_for(Gtk::Window &parent)
{
    window->set_transient_for(parent);
}


void OperationSearch::present()
{
    window->present();
}
