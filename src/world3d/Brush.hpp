/**
 * Brush.hpp - OpenGL Editor::Brush view.
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

#ifndef SE_WORLD3D_BRUSH_HPP
#define SE_WORLD3D_BRUSH_HPP

#include "DeferredExec.hpp"
#include "Face.hpp"
#include "RenderComponent.hpp"

#include <editor/world/Brush.hpp>

#include <glutils/glutils.hpp>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>

#include <functional>
#include <vector>
#include <memory>


namespace World3D
{
    /**
     * A component which can only be attached to a single Brush at a time.
     *
     * Renders a 3D view of the Brush using OpenGL.
     */
    class Brush : public DeferredExec, public RenderComponent
    {
    public:
        using PreDrawFunc = std::function<void(
            GLUtil::Program &,
            Sickle::Editor::Brush const *)>;

        static PreDrawFunc predraw;

        Brush()=default;
        virtual ~Brush()=default;

        /**
         * Render the view.
         *
         * @warning Requires an active OpenGL context.
         */
        void render() const;

        /**
         * Queues a render() call.
         */
        virtual void execute() override;

    protected:
        // TODO: Temporary solution since Face needs access to the shader.
        friend class Face;

        // WARNING: The first call to this requires an active OpenGL context.
        static GLUtil::Program &shader();

        // Component interface.
        virtual void on_attach(Sickle::Componentable &) override;
        // Component interface.
        virtual void on_detach(Sickle::Componentable &) override;

    private:
        Sickle::Editor::Brush const *_src{nullptr};
        std::vector<std::shared_ptr<Face>> _faces{};

        std::vector<sigc::connection> _signals{};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};

        Brush(Brush const &)=delete;
        Brush &operator=(Brush const &)=delete;

        /** @warning Requires an active OpenGL context. */
        void _init();
        /** @warning Requires an active OpenGL context. */
        void _sync_face(std::shared_ptr<Face> const &face);

        void _on_face_changed(std::shared_ptr<Face> const &face);
    };
}

#endif
