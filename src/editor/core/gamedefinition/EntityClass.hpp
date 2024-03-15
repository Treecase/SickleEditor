/**
 * EntityClass.hpp - Entity class information.
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

#ifndef SE_EDITOR_CORE_GAMEDEFINITION_ENTITYCLASS_HPP
#define SE_EDITOR_CORE_GAMEDEFINITION_ENTITYCLASS_HPP

#include "ClassProperties.hpp"
#include "EntityPropertyDefinition.hpp"

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>


namespace Sickle::Editor
{
    /**
     * Holds entity class information.
     */
    class EntityClass
    {
    public:
        EntityClass()=default;
        EntityClass(FGD::Class const &cls);

        /**
         * Class type (PointClass, SolidClass, etc.)
         *
         * @return The Class's type.
         */
        std::string type() const;

        /**
         * Check if the class has a property of type T.
         *
         * @tparam T Property type.
         * @return True if the class has the property, else false.
         */
        template<class T> bool has_class_property() const
        {
            return _class_properties.count(std::type_index{typeid(T)}) != 0;
        }

        /**
         * Get the class property of type T, or nullptr if the class does not
         * have this property.
         *
         * @tparam T Property type.
         * @return The property or nullptr.
         */
        template<class T> T *get_class_property() const
        {
            try {
                return dynamic_cast<T *>(
                    _class_properties.at(std::type_index{typeid(T)}).get());
            }
            catch (std::out_of_range const &e) {
                return nullptr;
            }
        }

        /**
         * Check if the class has an entity property matching the given name.
         *
         * @param name Name of the property to check.
         * @return True if the class has the property, else false.
         */
        bool has_property(std::string const &name) const;

        /**
         * Get the entity property definition identitified by name, or nullptr
         * if it doesn't exist.
         *
         * @param name Name of the property to get.
         * @return The property definition, or nullptr if not found.
         */
        std::shared_ptr<EntityPropertyDefinition>
        get_property(std::string const &name) const;

        /**
         * Get the entity properties.
         *
         * @return A collection of the entity properties.
         */
        std::vector<std::shared_ptr<EntityPropertyDefinition>>
        get_entity_properties() const;

        /**
         * Inherit properties from another class.
         *
         * @param other The class to inherit from.
         */
        void inherit_from(EntityClass const &other);

    protected:
        /// Attempt to add a class property. Fails silently if the slot is
        /// already filled.
        void add_class_property(std::shared_ptr<ClassProperty> const &property);
        /// Attempt to add an entity property. Fails silently if the property
        /// already exists.
        void add_entity_property(
            std::shared_ptr<EntityPropertyDefinition> const &property);

    private:
        std::string _type{"<undefined>"};
        std::unordered_map<
            std::type_index,
            std::shared_ptr<ClassProperty>> _class_properties{};
        std::unordered_map<
            std::string,
            std::shared_ptr<EntityPropertyDefinition>> _entity_properties{};
    };
}

#endif
