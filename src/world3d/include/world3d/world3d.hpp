/**
 * world3d.hpp - OpenGL Editor::World view.
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

#ifndef SE_WORLD3D_HPP
#define SE_WORLD3D_HPP

#include <world/EditorWorld.hpp>
#include <glutils/glutils.hpp>
#include <wad/lumps.hpp>
#include <wad/TextureManager.hpp>

#include <sigc++/trackable.h>

#include <memory>


namespace World3D
{
    struct Texture
    {
        std::shared_ptr<GLUtil::Texture> texture{nullptr};
        int width, height;

        /**
         * Get the "Missing Texture" texture. It will only be generated once,
         * and will be reused on subsequent calls.
         */
        static Texture make_missing_texture();

        Texture()=default;
        Texture(WAD::TexLump const &texlump);
    };


    class Brush;
    class Entity;
    class World3D;


    class Vertex
    {
        glm::vec3 position;
        glm::vec2 uv;

    public:
        static size_t const ELEMENTS = 5;
        std::array<GLfloat, ELEMENTS> as_vbo() const;

        Vertex(glm::vec3 pos, glm::vec2 uv);
    };

    class Face : public sigc::trackable
    {
        Sickle::Editor::FaceRef const _src{nullptr};

        static sigc::signal<void(std::string)> _signal_missing_texture;
        sigc::signal<void()> _signal_verts_changed{};

        void _sync_vertices();

        void _on_src_verts_changed();
        void _on_src_texture_changed();

    public:
        Texture texture{};
        std::vector<Vertex> vertices{};
        GLint offset; // offset into _PARENT's VBO.

        static auto &signal_missing_texture() {return _signal_missing_texture;}
        auto &signal_verts_changed() {return _signal_verts_changed;}

        /** Length of span in _PARENT's VBO. */
        GLsizei count() const {return vertices.size();}

        Face(
            Sickle::Editor::FaceRef face,
            GLint offset);
        Face(Face const &other);
    };

    class Brush : public sigc::trackable
    {
        Sickle::Editor::BrushRef const _src{nullptr};
        std::vector<Face> _faces{};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};

        void _on_face_changed(size_t face_index);

    public:
        bool is_selected() const;
        void render() const;

        Brush(Sickle::Editor::BrushRef const &src);
    };

    class Entity
    {
    public:
        std::vector<Brush> brushes{};

        void render() const;

        Entity(Sickle::Editor::Entity &src);
    };

    class World3D
    {
    public:
        std::vector<Entity> entities{};

        void render() const;

        World3D(Glib::RefPtr<Sickle::Editor::World> src);
    };
}

#endif
