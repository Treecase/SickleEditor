/**
 * Selection.cpp - Editor::Selection.
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

#include "Selection.hpp"

using namespace Sickle::Editor;

void Selection::clear()
{
    signal_updated().block();
    auto const copy = _selected;
    for (auto item : copy)
    {
        remove(item);
    }
    signal_updated().unblock();
    signal_updated().emit();
}

void Selection::add(Item item)
{
    if (contains(item))
    {
        return;
    }
    if (!item->is_selected())
    {
        item->select(true);
    }
    _selected.emplace(item);
    signal_updated().emit();
}

void Selection::remove(Item item)
{
    if (!contains(item))
    {
        return;
    }
    if (item->is_selected())
    {
        item->select(false);
    }
    _selected.erase(item);
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

std::vector<Selection::Item> Selection::get_all_of_type(
    std::type_info const &type) const
{
    std::vector<Item> items{};
    std::copy_if(
        begin(),
        end(),
        std::back_inserter(items),
        [&type](Item const &item) -> bool
        { return typeid(*item.get()) == type; });
    return items;
}

Selection::Item Selection::get_latest_of_type(std::type_info const &type) const
{
    auto const all = get_all_of_type(type);
    if (!all.empty())
    {
        return all.back();
    }
    return Item{};
}
