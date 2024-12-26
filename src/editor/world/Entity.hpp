/**
 * Entity.hpp - Editor::Entity.
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

#ifndef SE_EDITOR_WORLD_ENTITY_HPP
#define SE_EDITOR_WORLD_ENTITY_HPP

#include "Brush.hpp"

#include <editor/core/gamedefinition/EntityClass.hpp>
#include <editor/interfaces/EditorObject.hpp>
#include <files/map/map.hpp>
#include <files/rmf/rmf.hpp>

#include <glibmm.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Sickle::Editor
{
    class Entity;
    using EntityRef = Glib::RefPtr<Entity>;

    /**
     * Entities represent game objects in the world.
     *
     * Entities have a list of properties, which modify the entity's behaviour
     * in-game.
     *
     * All entities have a class. This defines the overall in-game behaviour of
     * the entity, for example 'func_door' or 'func_trigger'.
     */
    class Entity
    : public EditorObject
    , public Lua::Referenceable
    {
    public:
        static EntityRef create(std::string const &classname);
        static EntityRef create(MAP::Entity const &entity);
        static EntityRef create(RMF::Entity const &entity);

        virtual ~Entity();

        operator MAP::Entity() const;

        /** Emitted when a property is added, deleted, or changes value. */
        auto &signal_properties_changed() { return _signal_properties_changed; }

        /**
         * Get the entity's class information.
         *
         * @return Class information for the entity.
         */
        EntityClass classinfo() const;

        /**
         * Get the entity's classname property.
         *
         * @return Class name of the entity.
         */
        std::string classname() const;

        /**
         * Get the entity's properties.
         *
         * @return The entity's properties.
         */
        std::unordered_map<std::string, std::string> properties() const;

        /**
         * Get the entity property named `key`.
         *
         * @param key Name of the property to access.
         * @return The value of the property.
         * @throw std::out_of_range if key does not exist.
         */
        std::string get_property(std::string const &key) const;

        /**
         * Set the value of the entity property named `key`.
         *
         * @param key Name of the property to access.
         * @param value Value to set the property to.
         */
        void set_property(std::string const &key, std::string const &value);

        /**
         * Remove the entity property named `key`.
         *
         * @param key Name of the property to remove.
         */
        bool remove_property(std::string const &key);

        /**
         * Get a list of brushes associated with the entity.
         *
         * @return List of the entity's brushes.
         */
        std::vector<BrushRef> brushes() const;

        /**
         * Add a brush to the entity.
         *
         * @param brush The brush to add.
         */
        void add_brush(BrushRef const &brush);

        /**
         * Remove a brush from the entity.
         *
         * @param brush The brush to remove.
         */
        void remove_brush(BrushRef const &brush);

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<EditorObjectRef> children() const override;

    protected:
        Entity(std::string const &classname);

    private:
        struct Property
        {
            /// The type of the property.
            std::shared_ptr<EntityPropertyDefinition> type;
            /// The value of the property.
            std::string value;

            /**
             * Default construct the value using type->default_value().
             *
             * @param type The property's type.
             */
            explicit Property(
                std::shared_ptr<EntityPropertyDefinition> const &type)
            : type{type}
            , value{type->default_value()}
            {
            }

            /**
             * Construct the value.
             *
             * @param type The property's type.
             * @param value The value of the property.
             */
            explicit Property(
                std::shared_ptr<EntityPropertyDefinition> const &type,
                std::string const &value)
            : type{type}
            , value{value}
            {
            }
        };

        static std::shared_ptr<EntityPropertyDefinition> origin_definition;

        sigc::signal<void()> _signal_properties_changed{};

        EntityClass _classinfo{};
        std::string _classname;
        std::unordered_map<std::string, Property> _properties{};
        std::vector<BrushRef> _brushes{};

        void _on_classname_changed();
    };
} // namespace Sickle::Editor

#endif
