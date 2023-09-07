/**
 * MapToolConfig.cpp - Config box for MapTools.
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

#include "MapToolConfig.hpp"

#include <cstdlib>

using namespace Sickle::AppWin;


MapToolConfig::MapToolConfig()
:   Glib::ObjectBase{typeid(MapToolConfig)}
,   Gtk::Frame{}
{
    set_label("Tool Options");

    _confirm.signal_clicked().connect(signal_confirmed().make_slot());
    _confirm.signal_show().connect(
        [this](){_confirm.set_visible(has_operation());});
    _confirm.signal_hide().connect(
        [this](){_confirm.set_visible(has_operation());});

    _box.set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    _box.set_spacing(8);
    _box.set_margin_top(8);
    _box.pack_end(_confirm, Gtk::PackOptions::PACK_SHRINK);

    _scrolled_window.add(_box);

    add(_scrolled_window);
}


bool MapToolConfig::has_operation() const
{
    return (bool)_operation;
}


void MapToolConfig::set_operation(Editor::Operation const &op)
{
    _operation.reset(new Editor::Operation{op});
    _args.clear();

    for (auto &entry : _entries)
        _box.remove(entry);
    _entries.clear();

    for (size_t i = 0; i < _operation->arg_types.size(); ++i)
    {
        auto &entry = _entries.emplace_back();
        auto const type = _operation->arg_types.at(i);
        switch (type)
        {
        case 'f':{
            auto const arg = _args.emplace_back(std::make_any<lua_Number>());
            entry.set_text(std::to_string(std::any_cast<lua_Number>(arg)));
            break;}
        }
        entry.property_text().signal_changed().connect(
            sigc::bind(
                sigc::mem_fun(*this, &MapToolConfig::on_entry_changed),
                i));

        _box.add(entry);
    }
    _box.show_all_children();
    _confirm.show();
}


Sickle::Editor::Operation MapToolConfig::get_operation() const
{
    if (_operation)
        return *_operation;
    throw std::logic_error{"operation unset"};
}


void MapToolConfig::clear_operation()
{
    _operation.release();
    for (auto &entry : _entries)
        _box.remove(entry);
    _entries.clear();
    _confirm.hide();
}


Sickle::Editor::Operation::ArgList MapToolConfig::get_arguments() const
{
    return _args;
}



void MapToolConfig::on_entry_changed(size_t argument_index)
{
    auto const type = _operation->arg_types.at(argument_index);
    auto &arg = _args.at(argument_index);
    auto &entry = _entries.at(argument_index);

    auto const text = entry.get_text();

    switch (type)
    {
    case 'f':
        arg.emplace<lua_Number>(strtof(text.c_str(), nullptr));
        break;
    }
}
