/**
 * EditorObject.cpp - Interface for any object visible in the Outliner view.
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

#include "EditorObject.hpp"

using namespace Sickle::Editor;


EditorObject::EditorObject()
{
    signal_child_removed().connect(
        [](auto child){child->signal_removed().emit();});
    signal_child_added().connect(
        [](auto child){child->signal_added().emit();});
}


std::vector<Glib::RefPtr<EditorObject>> EditorObject::children_recursive() const
{
    auto out = children();
    for (auto const &child : children())
    {
        auto const grandchildren = child->children_recursive();
        out.insert(
            out.cend(),
            grandchildren.cbegin(),
            grandchildren.cend());
    }
    return out;
}


void EditorObject::foreach(SlotForEach func)
{
    for (auto &obj : children_recursive())
        std::invoke(func, obj);
}