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

#include <files/fgd/fgd.hpp>

#include <unordered_map>


namespace Sickle::Editor
{
    /**
     * Holds entity class information.
     */
    struct EntityClass
    {
        /// Class type (PointClass, SolidClass, etc.)
        std::string type;
        /// Class properties [base(), iconsprite(), etc.]
        std::unordered_map<std::string, std::string> properties;
        /// Entity properties. TODO
        std::unordered_map<std::string, std::string> entity_properties;
    };


    /**
     * Manages entity classes.
     */
    class GameDefinition
    {
    public:
        /** Get a reference to the GameDefinition singleton. */
        static GameDefinition &instance();

        GameDefinition();

        /** Add a game definition to the manager. */
        void add_game(FGD::GameDef const &game);

        /**
         * Look up an entity class.
         *
         * Throws KeyError if the class does not exist.
         */
        EntityClass lookup(std::string const &classname) const;

    private:
        std::unordered_map<std::string, EntityClass> _classes{};

        GameDefinition(GameDefinition const &)=delete;
        GameDefinition &operator=(GameDefinition const &)=delete;
    };
}

#endif
