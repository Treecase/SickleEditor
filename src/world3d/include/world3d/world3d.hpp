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

#include <editor/EditorWorld.hpp>
#include <glutils/glutils.hpp>
#include <wad/lumps.hpp>
#include <wad/TextureManager.hpp>

#include <sigc++/connection.h>

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

    using TextureManager = WAD::TextureManager<Texture>;


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

    class Face
    {
        Brush &_parent;
        std::shared_ptr<Sickle::Editor::Face> _src{nullptr};

        sigc::signal<void()> _signal_verts_changed{};

        void _sync_vertices();

        void _on_src_verts_changed();
        sigc::connection _verts_changed_connection;

    public:
        Texture texture{};
        std::vector<Vertex> vertices{};
        GLint offset; // offset into _PARENT's VBO.

        auto &signal_verts_changed() {return _signal_verts_changed;}

        TextureManager &texman() const;

        /** Length of span in _PARENT's VBO. */
        GLsizei count() const {return vertices.size();}

        Face(Brush &parent, std::shared_ptr<Sickle::Editor::Face> &face);
        Face(Face const &other);
        virtual ~Face();
    };

    class Brush
    {
        Entity &_parent;
        std::shared_ptr<Sickle::Editor::Brush> _src{nullptr};
        std::vector<Face> _faces{};

        std::shared_ptr<GLUtil::VertexArray> _vao{nullptr};
        std::shared_ptr<GLUtil::Buffer> _vbo{nullptr};

        void _on_face_changed(size_t face_index);
        std::vector<sigc::connection> _face_changed_connection;

    public:
        bool is_selected() const {return _src->is_selected;}
        void render() const;

        TextureManager &texman() const;

        Brush(Entity &p, std::shared_ptr<Sickle::Editor::Brush> &src);
        Brush(Brush const &other);
        virtual ~Brush();
    };

    class Entity
    {
        World3D &_parent;
    public:
        std::vector<Brush> brushes{};

        TextureManager &texman() const;
        void render() const;

        Entity(World3D &parent, Sickle::Editor::Entity &src);
    };

    class World3D
    {
    public:
        TextureManager texman{};
        std::vector<Entity> entities{};

        void render() const;

        World3D(Sickle::Editor::Map &src, std::vector<std::string> const &wads);
    };
}

#endif
