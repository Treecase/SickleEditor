/**
 * World.cpp - Editor::World.
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

#ifndef SE_EDITOR_WORLD_MAP_HPP
#define SE_EDITOR_WORLD_MAP_HPP

#include "Brush.hpp"
#include "Entity.hpp"

#include <map/map.hpp>
#include <rmf/rmf.hpp>

#include <glibmm.h>

#include <memory>
#include <vector>


namespace Sickle::Editor
{
    class World;
    using WorldRef = Glib::RefPtr<World>;

    class World : public EditorObject
    {
    public:
        static WorldRef create();
        static WorldRef create(MAP::Map const &map);
        static WorldRef create(RMF::RichMap const &map);

        World &operator=(World const &other);
        /** Convert to .map format. */
        operator MAP::Map() const;

        // WARNING: You are expected to not modify the collection itself, only
        // contained items.
        auto &entities() {return _entities;}
        auto &entities() const {return _entities;}

        void add_entity(EntityRef const &entity);
        void remove_brush(BrushRef const &brush);
        EntityRef worldspawn();

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<Glib::RefPtr<EditorObject>>
        children() const override;

    protected:
        World();

    private:
        sigc::signal<void()> _signal_changed{};
        std::vector<EntityRef> _entities{};
        // TODO:
        // - visgroups & groups
        // - paths (what are these?)
        // - cameras
    };
}

#endif
