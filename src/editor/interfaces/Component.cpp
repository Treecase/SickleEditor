/**
 * Component.cpp - Component interface.
 * Copyright (C) 2024 Trevor Last
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

#include "Component.hpp"

#include <algorithm>

using namespace Sickle;


std::vector<std::shared_ptr<Component>> Componentable::get_components() const
{
    return components;
}


std::vector<std::shared_ptr<Component>> Componentable::get_components_matching(
    std::function<bool(std::shared_ptr<Component>)> predicate) const
{
    std::vector<std::shared_ptr<Component>> matching{};
    std::copy_if(
        components.cbegin(), components.cend(),
        std::back_inserter(matching),
        predicate);
    return matching;
}


void Componentable::add_component(std::shared_ptr<Component> const &component)
{
    component->on_attach(*this);
    components.push_back(component);
}


void Componentable::remove_component(
    std::shared_ptr<Component> const &component)
{
    auto const it = std::find(components.begin(), components.end(), component);
    if (it != components.cend())
    {
        components.erase(it);
        (*it)->on_detach(*this);
    }
}
