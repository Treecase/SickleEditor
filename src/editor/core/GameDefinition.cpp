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
        EntityClass ec;
        ec.type = cls->type();
        for (auto const &class_property : cls->attributes)
        {
            ec.properties[class_property->name()] = class_property->inner();
        }
        for (auto const &entity_property : cls->properties)
        {
            ec.entity_properties[entity_property->name] =\
                entity_property->type();
        }
        _classes[cls->name] = ec;
    }
}


EntityClass GameDefinition::lookup(std::string const &classname) const
{
    return _classes.at(classname);
}
