/**
 * Entity.hpp - OpenGL Editor::Entity view.
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

#ifndef SE_WORLD3D_ENTITY_HPP
#define SE_WORLD3D_ENTITY_HPP

#include "DeferredExec.hpp"
#include "RenderComponent.hpp"

#include <editor/world/Entity.hpp>

#include <glutils/glutils.hpp>

#include <functional>
#include <memory>


namespace World3D
{
    /**
     * Entity view interface.
     */
    class EntityView : public DeferredExec, public RenderComponent
    {
    public:
        virtual ~EntityView()=default;
        virtual void render() const=0;
    };


    /**
     * Entity view for SolidClass entities.
     *
     * Renders as child brush geometry.
     */
    class SolidEntity : public EntityView
    {
    public:
        SolidEntity()=default;
        virtual ~SolidEntity()=default;

        virtual void render() const override;
        virtual void execute() override;

    protected:
        virtual void on_attach(Sickle::Componentable &) override;
        virtual void on_detach(Sickle::Componentable &) override;
    };


    /**
     * Entity view for PointClass entities.
     *
     * Renders as a solid-colored 3D box.
     */
    class PointEntityBox : public EntityView
    {
    public:
        struct ShaderParams
        {
            glm::mat4 model, view, projection;
        };

        using PreDrawFunc =\
            std::function<void(ShaderParams &, Sickle::Editor::Entity const *)>;

        static PreDrawFunc predraw;

        static GLUtil::Program &shader();

        PointEntityBox();
        virtual ~PointEntityBox()=default;

        /**
         * Render the box.
         *
         * @warning Requires an active OpenGL context.
         */
        virtual void render() const override;

        /** Queues a render() call. */
        virtual void execute() override;

    protected:
        virtual void on_attach(Sickle::Componentable &) override;
        virtual void on_detach(Sickle::Componentable &) override;

    private:
        static constexpr float DEFAULT_BOX_SIZE = 32.0f;

        Sickle::Editor::Entity const *_src{nullptr};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};
        std::shared_ptr<GLUtil::Buffer> _ebo{nullptr};

        glm::vec3 _color{1.0f, 1.0f, 0.0f};

        /** @warning Requires an active OpenGL context. */
        void _init_construct();
        /** @warning Requires an active OpenGL context. */
        void _init();
    };


    /**
     * Entity view for PointClass entities.
     *
     * Renders as a billboarded 2D sprite.
     */
    class PointEntitySprite : public EntityView
    {
    public:
        struct ShaderParams {
            glm::mat4 model, view, projection;
        };

        using PreDrawFunc =\
            std::function<void(
                ShaderParams &,
                Sickle::Editor::Entity const *)>;

        static PreDrawFunc predraw;
        static std::string sprite_root_path;

        /** @warning First call requires an active OpenGL context. */
        static GLUtil::Program &shader();

        PointEntitySprite();
        virtual ~PointEntitySprite()=default;

        /**
         * Render the sprite.
         *
         * @warning Requires an active OpenGL context.
         */
        virtual void render() const override;

        /** Queues a render() call. */
        virtual void execute() override;

    protected:
        virtual void on_attach(Sickle::Componentable &) override;
        virtual void on_detach(Sickle::Componentable &) override;

    private:
        Sickle::Editor::Entity const *_src{nullptr};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};
        std::shared_ptr<GLUtil::Texture> _sprite{nullptr};

        /** @warning Requires an active OpenGL context. */
        void _init_construct();
        /** @warning Requires an active OpenGL context. */
        void _init();
    };


    /**
     * Entity view for PointClass entities.
     *
     * Renders as a 3D model.
     */
    class PointEntityModel : public EntityView
    {
    public:
        virtual ~PointEntityModel()=default;
        void render() const override;
    };
}

#endif
