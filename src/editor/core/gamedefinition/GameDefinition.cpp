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
static std::vector<std::string> get_bases(std::shared_ptr<FGD::Class> cls);


GameDefinition &GameDefinition::instance()
{
    static GameDefinition singleton{};
    return singleton;
}


GameDefinition::GameDefinition()
{
}


void GameDefinition::add_game(FGD::GameDef const &game)
{
    for (auto const &cls : game.classes)
    {
        if (cls->type() == "BaseClass")
            continue;

        EntityClass ec{cls->type()};
        _merge_class(ec, cls, game);

        _classes.insert({cls->name, ec});
    }
}


EntityClass GameDefinition::lookup(std::string const &classname) const
{
    return _classes.at(classname);
}



void GameDefinition::_merge_class(
    EntityClass &ec,
    std::shared_ptr<FGD::Class> cls,
    FGD::GameDef const &game)
{
    if (!cls)
        return;

    for (auto const &class_property : cls->attributes)
    {
        ec.add_class_property(
            ClassPropertyFactory::construct(class_property));
    }
    for (auto const &entity_property : cls->properties)
    {
        ec.add_entity_property(
            EntityPropertyFactory::construct(entity_property));
    }

    auto const bases = get_bases(cls);
    for (auto const base : bases)
    {
        auto const base_cls = get_class_by_name(game, base);
        _merge_class(ec, base_cls, game);
    }
}



static std::shared_ptr<FGD::Class> get_class_by_name(
    FGD::GameDef const &game,
    std::string const &name)
{
    for (auto const &cls : game.classes)
        if (cls->name == name)
            return cls;
    return nullptr;
}


static std::vector<std::string> get_bases(std::shared_ptr<FGD::Class> cls)
{
    for (auto const &cp : cls->attributes)
    {
        auto const bcp = std::dynamic_pointer_cast<FGD::BaseAttribute>(cp);
        if (bcp)
            return bcp->baseclasses;
    }
    return {};
}
