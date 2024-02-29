/**
 * World.cpp - Editor::World.
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

#ifndef SE_EDITOR_WORLD_MAP_HPP
#define SE_EDITOR_WORLD_MAP_HPP

#include "Brush.hpp"
#include "Entity.hpp"

#include <files/map/map.hpp>
#include <files/rmf/rmf.hpp>

#include <glibmm.h>

#include <memory>
#include <vector>


namespace Sickle::Editor
{
    class World;
    using WorldRef = Glib::RefPtr<World>;

    /**
     * World represents a toplevel 'Map'.
     *
     * Worlds consist of a tree-like structure, with the world acting as root.
     * The next layer are the Entities, then the Brushes.
     *
     * All worlds have at least one entity, the worldspawn. This entity
     * contains all the physical world geometry.
     */
    class World : public EditorObject
    {
    public:
        static WorldRef create();
        static WorldRef create(MAP::Map const &map);
        static WorldRef create(RMF::RichMap const &map);

        virtual ~World();

        operator MAP::Map() const;

        /**
         * Get a list of entities in the world.
         *
         * @return A list of entities in the World.
         */
        auto &entities() const {return _entities;}

        /**
         * Add an entity to the world.
         *
         * @param entity The entity to add.
         * @throw std::logic_error if entity->classname() is "worldspawn".
         */
        void add_entity(EntityRef const &entity);

        /**
         * Remove an entity from the world.
         *
         * @param entity The entity to remove.
         */
        void remove_entity(EntityRef const &entity);

        /**
         * Remove a brush from the world.
         *
         * @param entity The entity to remove.
         * @deprecated Will be removed
         */
        void remove_brush(BrushRef const &brush);

        /**
         * Get the world's worldspawn entity.
         *
         * @return The worldspawn entity.
         */
        EntityRef worldspawn();

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<EditorObjectRef> children() const override;

    protected:
        World();

    private:
        EntityRef _worldspawn{nullptr};
        std::vector<EntityRef> _entities{};
        sigc::connection _conn_worldspawn_removed{};

        void _on_worldspawn_removed();
        void _add_worldspawn();
        void _replace_worldspawn(EntityRef const &entity);
    };
}

#endif
