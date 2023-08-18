/**
 * Selection.cpp - Editor::Selection.
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

#include "core/Editor.hpp"

using namespace Sickle::Editor;


void Selection::clear()
{
    signal_updated().block();
    auto const copy = _selected;
    for (auto item : copy)
        remove(item);
    signal_updated().unblock();
    signal_updated().emit();
}


void Selection::add(Item item)
{
    _selected.emplace(item);
    item->is_selected = true;
    _selected_signals[item] = {};
    for (auto &face : item->faces)
        _selected_signals[item].emplace_back(
            face->signal_vertices_changed().connect(
                signal_updated().make_slot()));
    signal_updated().emit();
}


void Selection::remove(Item item)
{
    _selected.erase(item);
    item->is_selected = false;
    for (auto &conn : _selected_signals[item])
        conn.disconnect();
    _selected_signals.erase(item);
    signal_updated().emit();
}


bool Selection::contains(Item item) const
{
    return _selected.count(item) != 0;
}

bool Selection::empty() const
{
    return _selected.empty();
}
