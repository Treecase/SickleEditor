/**
 * GameDefinition.cpp - Entity types management.
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

#include "GameDefinition.hpp"

using namespace Sickle::Editor;

// Get a class from an fgd by name.
static std::shared_ptr<FGD::Class> get_class_by_name(
    FGD::GameDef const &game,
    std::string const &name);

// Get the base classes from an fgd class.
static std::vector<std::string> get_bases(FGD::Class const &cls);

GameDefinition &GameDefinition::instance()
{
    static GameDefinition singleton{};
    return singleton;
}

GameDefinition::GameDefinition() {}

void GameDefinition::add_game(FGD::GameDef const &game)
{
    for (auto const &cls : game.classes)
    {
        // BaseClasses cannot be instantiated.
        if (cls->type() == "BaseClass")
        {
            continue;
        }
        auto const ec = _instantiate_class(*cls, game);
        _classes.insert({cls->name, ec});
    }
}

EntityClass GameDefinition::lookup(std::string const &classname) const
{
    return _classes.at(classname);
}

std::unordered_set<std::string> GameDefinition::get_all_classnames() const
{
    std::unordered_set<std::string> classnames{};
    for (auto const &kv : _classes)
    {
        classnames.insert(kv.first);
    }
    return classnames;
}

EntityClass GameDefinition::_instantiate_class(
    FGD::Class const &cls,
    FGD::GameDef const &game)
{
    EntityClass ec{cls};
    // Resolve inheritance hierarchy.
    auto const bases = get_bases(cls);
    for (auto const &name : bases)
    {
        auto const base = get_class_by_name(game, name);
        auto const base_ec = _instantiate_class(*base, game);
        ec.inherit_from(base_ec);
    }
    return ec;
}

static std::shared_ptr<FGD::Class> get_class_by_name(
    FGD::GameDef const &game,
    std::string const &name)
{
    for (auto const &cls : game.classes)
    {
        if (cls->name == name)
        {
            return cls;
        }
    }
    return nullptr;
}

static std::vector<std::string> get_bases(FGD::Class const &cls)
{
    for (auto const &cp : cls.attributes)
    {
        auto const bcp = std::dynamic_pointer_cast<FGD::BaseAttribute>(cp);
        if (bcp)
        {
            return bcp->baseclasses;
        }
    }
    return {};
}
