/**
 * Brush.hpp - OpenGL Editor::Brush view.
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

#ifndef SE_WORLD3D_BRUSH_HPP
#define SE_WORLD3D_BRUSH_HPP

#include "DeferredExec.hpp"
#include "Face.hpp"

#include <glutils/glutils.hpp>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>
#include <world/Brush.hpp>

#include <functional>
#include <vector>
#include <memory>


namespace World3D
{
    class Brush : public sigc::trackable, public DeferredExec
    {
    public:
        using PreDrawFunc = std::function<void(Sickle::Editor::BrushRef const)>;

        static PreDrawFunc predraw;

        Brush(Sickle::Editor::BrushRef const &src);

        auto &signal_deleted() {return _sig_deleted;}

        bool is_selected() const;

        /** @warning Requires an active OpenGL context. */
        void render() const;

    private:
        Sickle::Editor::BrushRef const _src{nullptr};
        std::vector<std::shared_ptr<Face>> _faces{};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};

        sigc::signal<void()> _sig_deleted{};

        Brush(Brush const &)=delete;
        Brush &operator=(Brush const &)=delete;

        /** @warning Requires an active OpenGL context. */
        void _init();
        /** @warning Requires an active OpenGL context. */
        void _sync_face(std::shared_ptr<Face> const &face);

        void _on_face_changed(std::shared_ptr<Face> const &face);
        void _on_real_changed();
    };
}

#endif
