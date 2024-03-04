/**
 * EntityProperties.hpp - Defines the entity property types.
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

#ifndef SE_EDITOR_CORE_GAMEDEFINITION_ENTITYPROPERTIES_HPP
#define SE_EDITOR_CORE_GAMEDEFINITION_ENTITYPROPERTIES_HPP

#include <files/fgd/fgd.hpp>

#include <memory>
#include <string>


namespace Sickle::Editor
{
    /**
     * An entity property definition.
     *
     * This describes a property attached to each instance of an entity.
     */
    struct EntityProperty
    {
        EntityProperty(
            std::string const &name,
            std::string const &default_value);
        virtual ~EntityProperty()=default;

        /**
         * Get the property's name.
         *
         * @return The property name.
         */
        virtual std::string name() const;

        /**
         * Get the default value of the property.
         *
         * @return The property's default value.
         */
        virtual std::string default_value() const;

    private:
        std::string _name;
        std::string _default_value;
    };



    /**
     * Factory to construct EntityProperties.
     */
    struct EntityPropertyFactory
    {
        /**
         * Construct an EntityProperty for the given FGD::Property.
         *
         * @param prop The property to construct for.
         * @return The constructed property.
         */
        static std::shared_ptr<EntityProperty> construct(
            std::shared_ptr<FGD::Property> const &prop);
    };
}

#endif
