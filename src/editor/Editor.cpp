/**
 * Editor.cpp - Map Editor class.
 * Copyright (C) 2022 Trevor Last
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

#include "editor/Editor.hpp"

#include <algorithm>


/* ===[ Editor::BBox ]=== */
void Sickle::Editor::BBox::p1(MAP::Vertex v)
{
    _p1 = v;
    signal_updated().emit();
}

void Sickle::Editor::BBox::p2(MAP::Vertex v)
{
    _p2 = v;
    signal_updated().emit();
}


/* ===[ Editor::Selection ]=== */
void Sickle::Editor::Selection::clear() {
    std::for_each(
        _selected.begin(), _selected.end(),
        [](auto brush){brush->is_selected = false;});
    _selected.clear();
    signal_updated().emit();
}

void Sickle::Editor::Selection::add(Item *item) {
    _selected.emplace(item);
    item->is_selected = true;
    signal_updated().emit();
}

void Sickle::Editor::Selection::remove(Item *item) {
    _selected.erase(item);
    item->is_selected = false;
    signal_updated().emit();
}

bool Sickle::Editor::Selection::contains(Item *item) const {
    return _selected.count(item) != 0;
}
