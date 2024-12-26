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

#include <editor/interfaces/EditorObject.hpp>
#include <files/map/map.hpp>
#include <files/rmf/rmf.hpp>
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

    /**
     * Brushes describe the shape of an entity in the world.
     *
     * Each brush is a convex 3D polygon. Brushes are associated with an
     * Entity, which determines how the child brush(es) are interpreted. For
     * example, the worldspawn entity treats them as solid world geometry,
     * whereas a func_trigger treats it as a hitbox.
     */
    class Brush
    : public EditorObject
    , public Lua::Referenceable
    {
    public:
        /**
         * Create a new empty brush. You probably don't want to do this.
         */
        static BrushRef create();

        /**
         * Create a new brush from a set of 3D points. Throws if these points
         * do not make up a valid convex 3D polygon.
         */
        static BrushRef create(std::vector<glm::vec3> const &points);

        /**
         * Create a new brush from MAP format brush data.
         */
        static BrushRef create(MAP::Brush const &brush);

        /**
         * Create a new brush from RMF format brush data.
         */
        static BrushRef create(RMF::Solid const &solid);

        virtual ~Brush();

        operator MAP::Brush() const;

        /**
         * Get a list of faces associated with this brush.
         *
         * @return A list of the brush's faces.
         */
        std::vector<FaceRef> faces() const;

        /**
         * Transform the brush by `matrix`.
         *
         * @param matrix Transformation matrix to apply.
         */
        void transform(glm::mat4 const &matrix);

        /**
         * Translate the brush by `translation`.
         *
         * @param translation Vector to translate by.
         */
        void translate(glm::vec3 const &translation);

        // EditorObject interface
        virtual Glib::ustring name() const override;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const override;
        virtual std::vector<EditorObjectRef> children() const override;

    protected:
        Brush();

        void on_removed();

    private:
        std::vector<FaceRef> _faces{};
        // TODO:
        // - visgroup id
        // - color
        // ^ these are only used by worldspawn brushes?
    };
} // namespace Sickle::Editor

#endif
