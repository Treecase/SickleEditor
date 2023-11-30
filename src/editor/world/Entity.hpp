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

    class Entity : public Glib::Object, public EditorObject
    {
    public:
        std::unordered_map<std::string, std::string> properties{};

        static EntityRef create();
        static EntityRef create(MAP::Entity const &entity);
        static EntityRef create(RMF::Entity const &entity);

        operator MAP::Entity() const;
        Entity &operator=(Entity const &other);

        auto &signal_changed() {return _signal_changed;}

        std::vector<BrushRef> brushes() const;

        void add_brush(BrushRef const &brush);
        void remove_brush(BrushRef const &brush);

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<EditorObject *> children() const override;

    protected:
        Entity();

    private:
        sigc::signal<void()> _signal_changed{};
        std::vector<BrushRef> _brushes{};
        // TODO:
        // - visgroup id
        // - color
    };
}

#endif
