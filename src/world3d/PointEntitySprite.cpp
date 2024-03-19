/**
 * PointEntitySprite.hpp - OpenGL Editor::Entity view for PointClass entities.
 *                         Renders as a billboarded sprite.
 * Copyright (C) 2024 Trevor Last
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

#include "Entity.hpp"

#include <files/spr/spr.hpp>
#include <utils/gtkglutils.hpp>

#include <giomm/datainputstream.h>
#include <giomm/file.h>

#include <iostream>


using namespace World3D;


// Feeds data from a Gio::File to the sprite loader.
class GioFileSpriteStream : public SPR::SpriteStream
{
public:
    GioFileSpriteStream(Glib::RefPtr<Gio::File> const &file);

    virtual uint8_t read_byte() override;
    virtual uint16_t read_uint16() override;
    virtual int32_t read_int32() override;
    virtual uint32_t read_uint32() override;
    virtual float read_float() override;
    virtual uint8_t *read_bytes(size_t count) override;

private:
    Glib::RefPtr<Gio::DataInputStream> _stream{nullptr};
};


// Get the "missing texture" sprite.
static std::shared_ptr<GLUtil::Texture> missing_texture();

// Load sprite data into an OpenGL texture object.
static std::shared_ptr<GLUtil::Texture> frame_to_texture(
    SPR::Frame const &frame,
    SPR::Palette const &palette);



PointEntitySprite::PreDrawFunc PointEntitySprite::predraw = [](auto, auto){};
std::string PointEntitySprite::sprite_root_path = ".";
std::string PointEntitySprite::game_root_path = ".";


GLUtil::Program &PointEntitySprite::shader()
{
    static GLUtil::Program the_shader{
        std::vector{
            GLUtil::shader_from_resource(
                "shaders/billboard.vert",
                GL_VERTEX_SHADER),
            GLUtil::shader_from_resource(
                "shaders/transparent.frag",
                GL_FRAGMENT_SHADER),
        },
        "PointEntitySpriteShader"
    };
    return the_shader;
}



PointEntitySprite::PointEntitySprite()
{
    push_queue([this](){_init_construct();});
}


void PointEntitySprite::render() const
{
    if (!_vao || !_sprite)
        return;

    std::stringstream origin_str{_src->get_property("origin")};
    glm::vec3 origin{};
    origin_str >> origin.x >> origin.y >> origin.z;

    ShaderParams params{};
    params.model = glm::identity<glm::mat4>();
    params.view = glm::identity<glm::mat4>();
    params.projection = glm::identity<glm::mat4>();

    predraw(params, _src);

    shader().use();
    shader().setUniformS("scale", glm::vec2{0.1f, 0.1f});
    shader().setUniformS("position", origin);
    shader().setUniformS("model", params.model);
    shader().setUniformS("view", params.view);
    shader().setUniformS("projection", params.projection);
    shader().setUniformS("tex", 0);
    shader().setUniformS("modulate",
        _src->is_selected()
        ? glm::vec3{1.0f, 0.0f, 0.0f}
        : glm::vec3{1.0f, 1.0f, 1.0f});

    glActiveTexture(GL_TEXTURE0);
    _sprite->bind();

    _vao->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _vao->unbind();
}


void PointEntitySprite::execute()
{
    push_queue([this](){render();});
}



void PointEntitySprite::on_attach(Sickle::Componentable &obj)
{
    if (_src)
        throw std::logic_error{"already attached"};

    _src = &dynamic_cast<Sickle::Editor::Entity &>(obj);
    if (_src->classinfo().type() != "PointClass")
        throw std::invalid_argument{"must be PointClass"};

    // Entity class must have either iconsprite() or sprite() property, but not
    // both.
    bool const has_iconsprite = _src->classinfo()
        .has_class_property<Sickle::Editor::ClassPropertyIconsprite>();
    bool const has_sprite = _src->classinfo()
        .has_class_property<Sickle::Editor::ClassPropertySprite>();
    if (has_iconsprite && has_sprite)
    {
        throw std::invalid_argument{
            "conflict: has both iconsprite() and sprite() properties"};
    }
    else if (!has_iconsprite && !has_sprite)
    {
        throw std::invalid_argument{
            "must have either an iconsprite() or a sprite() class property"};
    }

    if (has_sprite)
    {
        _src->signal_properties_changed().connect(
            sigc::mem_fun(
                *this,
                &PointEntitySprite::_on_src_properties_changed));
    }

    _is_iconsprite = has_iconsprite;
    push_queue([this](){_init();});
}


void PointEntitySprite::on_detach(Sickle::Componentable &obj)
{
    _src = nullptr;
    _sprite.reset();
    _sprite_path.reset();
    clear_queue();
}



void PointEntitySprite::_init_construct()
{
    std::vector<GLfloat> const vbo_data{
        // position |  UVs
        -0.5f, +0.5f,  0.0f, 0.0f, // left top
        -0.5f, -0.5f,  0.0f, 1.0f, // left bottom
        +0.5f, +0.5f,  1.0f, 0.0f, // right top
        +0.5f, -0.5f,  1.0f, 1.0f, // right bottom
    };

    _vao = std::make_shared<GLUtil::VertexArray>();
    _vbo = std::make_shared<GLUtil::Buffer>(GL_ARRAY_BUFFER);

    _vao->bind();
    _vbo->bind();
    _vbo->buffer(GL_STATIC_DRAW, vbo_data);
    _vao->enableVertexAttribArray(0, 2, GL_FLOAT, 4 * sizeof(GLfloat));
    _vao->enableVertexAttribArray(
        1,
        2,
        GL_FLOAT,
        4 * sizeof(GLfloat),
        2 * sizeof(GLfloat));
    _vbo->unbind();
    _vao->unbind();
}


void PointEntitySprite::_init()
{
    if (_is_iconsprite)
    {
        // TODO: Only load textures once and share the data.
        auto const iconsprite =\
            _src->classinfo()
                .get_class_property<Sickle::Editor::ClassPropertyIconsprite>();
        auto const path =\
            sprite_root_path + "/" + iconsprite->get_path();
        _load_sprite(path);
    }
    else
    {
        _sprite_update();
    }
}


void PointEntitySprite::_load_sprite(std::string const &path)
{
    std::unique_ptr<GioFileSpriteStream> sprite_stream{nullptr};
    try {
        sprite_stream = std::make_unique<GioFileSpriteStream>(
            Gio::File::create_for_path(path));
    }
    catch (Gio::Error const &e) {
        std::cerr << e.what() << std::endl;
        _sprite = missing_texture();
        return;
    }

    std::unique_ptr<SPR::Sprite> sprite{nullptr};
    try {
        sprite = std::make_unique<SPR::Sprite>(
            SPR::load_sprite(*sprite_stream));
    }
    catch (SPR::LoadError const &e) {
        std::cerr << "Error loading " << path << ": " << e.what() << std::endl;
        _sprite = missing_texture();
        return;
    }

    _sprite = frame_to_texture(sprite->frames.at(0), sprite->palette);
}


void PointEntitySprite::_sprite_update()
{
    auto const model = _src->get_property("model");
    if (_sprite_path.has_value() && model == _sprite_path.value())
        return;
    _sprite_path = model;
    auto const path = game_root_path + "/" + model;
    push_queue([this, path](){_load_sprite(path);});
}


void PointEntitySprite::_on_src_properties_changed()
{
    _sprite_update();
}



static std::shared_ptr<GLUtil::Texture> missing_texture()
{
    static std::shared_ptr<GLUtil::Texture> missing{nullptr};
    if (missing)
        return missing;

    constexpr GLsizei WIDTH = 7;
    constexpr GLsizei HEIGHT = 9;
    static constexpr uint8_t data[WIDTH * HEIGHT * 4] = {
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x80, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    };

    missing = std::make_shared<GLUtil::Texture>(GL_TEXTURE_2D);
    missing->bind();
    missing->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    missing->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    missing->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    missing->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        missing->type(), 0, GL_RGBA,
        WIDTH, HEIGHT, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);
    missing->unbind();
    return missing;
}


static std::shared_ptr<GLUtil::Texture> frame_to_texture(
    SPR::Frame const &frame,
    SPR::Palette const &palette)
{
    uint64_t const frame_size = frame.w * frame.h;
    auto rgba_data = std::make_unique<GLubyte[]>(frame_size * 4);
    for (uint64_t i = 0, j = 0; i < frame_size; ++i, j += 4)
    {
        auto const idx = frame.data[i];
        auto const &color = palette.colors.at(idx);
        rgba_data[j+0] = color.r;
        rgba_data[j+1] = color.g;
        rgba_data[j+2] = color.b;
        rgba_data[j+3] = color.a;
    }

    auto texture = std::make_shared<GLUtil::Texture>(GL_TEXTURE_2D);
    texture->bind();
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        texture->type(), 0, GL_RGBA,
        frame.w, frame.h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, rgba_data.get());
    texture->unbind();
    return texture;
}



/* ===[ Gio::File SpriteStream ]=== */
GioFileSpriteStream::GioFileSpriteStream(Glib::RefPtr<Gio::File> const &file)
:   _stream{Gio::DataInputStream::create(file->read())}
{
    _stream->set_byte_order(
        Gio::DataStreamByteOrder::DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN);
}

uint8_t GioFileSpriteStream::read_byte()
{
    return _stream->read_byte();
}

uint16_t GioFileSpriteStream::read_uint16()
{
    return _stream->read_uint16();
}

int32_t GioFileSpriteStream::read_int32()
{
    return _stream->read_int32();
}

uint32_t GioFileSpriteStream::read_uint32()
{
    return _stream->read_uint32();
}

float GioFileSpriteStream::read_float()
{
    uint8_t value_raw[4];
    value_raw[0] = read_byte();
    value_raw[1] = read_byte();
    value_raw[2] = read_byte();
    value_raw[3] = read_byte();
    return *reinterpret_cast<float *>(value_raw);
}

uint8_t *GioFileSpriteStream::read_bytes(size_t count)
{
    uint8_t *bytes = new uint8_t[count];
    gsize total_bytes_read = 0;
    while (total_bytes_read < count)
    {
        gsize bytes_read;
        _stream->read_all(bytes + total_bytes_read, 4, bytes_read);
        total_bytes_read += bytes_read;
    }
    return bytes;
}
