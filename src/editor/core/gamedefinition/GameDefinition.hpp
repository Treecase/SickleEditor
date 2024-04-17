/**
 * GameDefinition.hpp - Entity types management.
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

#ifndef SE_EDITOR_CORE_GAMEDEFINITION_HPP
#define SE_EDITOR_CORE_GAMEDEFINITION_HPP

#include "EntityClass.hpp"

#include <files/fgd/fgd.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>


namespace Sickle::Editor
{
    /**
     * Singleton which manages entity class definitions.
     */
    class GameDefinition
    {
    public:
        /**
         * Get a reference to the GameDefinition singleton.
         *
         * @return Reference to the GameDefintion singleton.
         */
        static GameDefinition &instance();

        /**
         * Add a game definition to the manager.
         *
         * @param game The game to add.
         */
        void add_game(FGD::GameDef const &game);

        /**
         * Look up an entity class.
         *
         * @param classname Name of the class to look up.
         * @return The class information.
         * @throw std::out_of_range if the class does not exist.
         */
        EntityClass lookup(std::string const &classname) const;

        /**
         * Get a list of all the defined classnames.
         *
         * @return A collection of all the defined classnames.
         */
        std::unordered_set<std::string> get_all_classnames() const;

    private:
        std::unordered_map<std::string, EntityClass> _classes{};

        static EntityClass _instantiate_class(
            FGD::Class const &cls,
            FGD::GameDef const &game);

        GameDefinition();
        GameDefinition(GameDefinition const &)=delete;
        GameDefinition &operator=(GameDefinition const &)=delete;
    };
}

#endif
