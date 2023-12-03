/**
 * Entity.hpp - Editor::Entity.
 * Copyright (C) 2023 Trevor Last
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

#include <interfaces/EditorObject.hpp>
#include <map/map.hpp>
#include <rmf/rmf.hpp>

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
     * All entities have at least one property, "classname". This property
     * defines the overall in-game behaviour of the entity, for example
     * 'func_door' or 'func_trigger'.
     */
    class Entity : public EditorObject
    {
    public:
        static EntityRef create();
        static EntityRef create(MAP::Entity const &entity);
        static EntityRef create(RMF::Entity const &entity);

        operator MAP::Entity() const;

        /** Emitted when a brush is added or removed. */
        auto &signal_changed() {return _signal_changed;}

        /** Get the entity's properties. */
        std::unordered_map<std::string, std::string> properties() const;

        /**
         * Get the entity property named `key`.
         *
         * @param key Name of the property to access.
         * @return The value of the property.
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
        virtual std::vector<Glib::RefPtr<EditorObject>>
        children() const override;

    protected:
        Entity();

    private:
        sigc::signal<void()> _signal_changed{};

        std::unordered_map<std::string, std::string> _properties{};
        std::vector<BrushRef> _brushes{};
        // TODO:
        // - visgroup id
        // - color
    };
}

#endif
