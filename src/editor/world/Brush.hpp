/**
 * Brush.hpp - Editor::Brush.
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

#ifndef SE_EDITOR_WORLD_BRUSH_HPP
#define SE_EDITOR_WORLD_BRUSH_HPP

#include "Face.hpp"

#include <interfaces/EditorObject.hpp>
#include <map/map.hpp>
#include <rmf/rmf.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <glibmm.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>


namespace Sickle::Editor
{
    class Brush;
    using BrushRef = Glib::RefPtr<Brush>;

    class Brush :
        public Glib::Object,
        public Lua::Referenceable,
        public EditorObject
    {
    public:
        std::vector<FaceRef> faces{};

        static BrushRef create();
        static BrushRef create(std::vector<glm::vec3> const &points);
        static BrushRef create(MAP::Brush const &brush);
        static BrushRef create(RMF::Solid const &solid);

        operator MAP::Brush() const;

        auto property_selected() {return _prop_selected.get_proxy();}
        auto property_selected() const {return _prop_selected.get_proxy();}
        auto property_real() {return _prop_real.get_proxy();}
        auto property_real() const {return _prop_real.get_proxy();}

        auto is_selected() const {return property_selected().get_value();}
        auto is_real() const {return property_real().get_value();}

        void transform(glm::mat4 const &matrix);
        void translate(glm::vec3 const &translation);

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<EditorObject *> children() const override;

    private:
        Glib::Property<bool> _prop_selected;
        Glib::Property<bool> _prop_real;

        Brush();
        // TODO:
        // - visgroup id
        // - color
        // ^ these are only used by worldspawn brushes?
    };
}

#endif
