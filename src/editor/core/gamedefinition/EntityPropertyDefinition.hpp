/**
 * EntityPropertyDefinition.hpp - Defines the entity property types.
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

#ifndef SE_EDITOR_CORE_GAMEDEFINITION_ENTITYPROPERTYDEFINITION_HPP
#define SE_EDITOR_CORE_GAMEDEFINITION_ENTITYPROPERTYDEFINITION_HPP

#include <files/fgd/fgd.hpp>

#include <memory>
#include <string>


namespace Sickle::Editor
{
    // TODO: Probably temporary. Encode this info implicitly by polymorphism.
    enum PropertyType
    {
        CHOICES,
        FLAGS,
        INTEGER,
        STRING,
        NUM_PROPERTYTYPES
    };

    /**
     * An entity property definition.
     *
     * This describes a property attached to each instance of an entity.
     */
    struct EntityPropertyDefinition
    {
        EntityPropertyDefinition(
            std::string const &name,
            std::string const &default_value,
            PropertyType type);
        virtual ~EntityPropertyDefinition()=default;

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

        /**
         * Get the type of the property.
         *
         * @return The property's default value.
         */
        virtual PropertyType type() const;

    private:
        std::string _name;
        std::string _default_value;
        PropertyType _type;
    };

    /**
     * Choice properties are stored as an integer, which can only take certain
     * defined valid values.
     */
    struct EntityPropertyDefinitionChoices : public EntityPropertyDefinition
    {
        EntityPropertyDefinitionChoices(
            std::string const &name,
            std::string const &default_value,
            std::map<int, std::string> const &choices);
        virtual ~EntityPropertyDefinitionChoices()=default;

        /**
         * Get the mapping of valid integer values and their descriptions.
         *
         * @return A map of integer keys and description strings.
         */
        auto &choices() const {return _choices;}

    private:
        std::map<int, std::string> _choices{};
    };

    /**
     * Flag properties are stored as an integer, interpreted as bitwise OR'd
     * flags.
     */
    struct EntityPropertyDefinitionFlags : public EntityPropertyDefinition
    {
        EntityPropertyDefinitionFlags(
            std::string const &name,
            std::map<int, std::pair<std::string, bool>> const &flags);
        virtual ~EntityPropertyDefinitionFlags()=default;

        /**
         * Get the description for a bit. The description is the blank string
         * for undefined bits.
         *
         * @param bit The bit number. Note that this starts at 1 and goes to 32.
         * @return The description string.
         */
        std::string get_description(int bit) const;

    private:
        struct FlagDef
        {
            std::string description;
            bool enabled_by_default;
            FlagDef(std::pair<std::string, bool> const &flagdef);
        };
        std::map<int, FlagDef> _flags{};
    };



    /**
     * Factory to construct EntityProperties.
     */
    struct EntityPropertyDefinitionFactory
    {
        /**
         * Construct an EntityPropertyDefinition for the given FGD::Property.
         *
         * @param prop The property to construct for.
         * @return The constructed property.
         */
        static std::shared_ptr<EntityPropertyDefinition> construct(
            std::shared_ptr<FGD::Property> const &prop);
    };
}

#endif
