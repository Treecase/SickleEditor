/**
 * MapArea.cpp - Sickle editor main window GLArea.
 * Copyright (C) 2022 Trevor Last
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

#include "MapArea.hpp"

#include "appid.hpp"


namespace GLUtil
{
    Shader shader_from_resource(std::string const &path, GLenum type)
    {
        auto b = Gio::Resource::lookup_data_global(SE_GRESOURCE_PREFIX + path);
        gsize size = 0;
        return {type, static_cast<char const *>(b->get_data(size)), path};
    }
}


/* ===[ MapArea ]=== */
Sickle::MapArea::MapArea()
:   Glib::ObjectBase{typeid(MapArea)}
,   Gtk::GLArea{}
,   _glmap{}
,   _shader{nullptr}
,   _camera{}
,   _transform{
        {0.0f, 0.0f, 0.0f},
        {glm::radians(-90.0f), 0.0f, 0.0f},
        {0.005f, 0.005f, 0.005f}}
,   _state{
        0.0, 0.0,
        0,
        {0.0f, 0.0f, 0.0f},
        false
    }
,   _prop_wireframe{false}
,   _prop_shift_multiplier{2.0f}
,   _prop_mouse_sensitivity{0.75f}
{
    set_required_version(4, 3);
    set_use_es(false);
    set_has_depth_buffer(true);
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_auto_render(true);
    set_can_focus(true);

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::SCROLL_MASK);

    add_tick_callback(sigc::mem_fun(*this, &MapArea::tick_callback));
}

void Sickle::MapArea::set_map(MAP::Map const *map)
{
    make_current();
    if (map)
        _glmap = MAP::GLMap{*map};
    else
        _glmap = MAP::GLMap{};
    queue_render();
}

void Sickle::MapArea::on_realize()
{
    Gtk::GLArea::on_realize();
    make_current();
    throw_if_error();

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        throw std::runtime_error{
            "glewInit - " + std::string{(char *)glewGetErrorString(error)}};
    }
    if (glewIsSupported("GL_VERSION_4_3") == GL_FALSE)
        throw std::runtime_error{"GLEW: OpenGL Version 4.3 not supported"};

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    _shader.reset(
        new GLUtil::Program{
            {
                GLUtil::shader_from_resource(
                    "shaders/map.vert", GL_VERTEX_SHADER),
                GLUtil::shader_from_resource(
                    "shaders/map.frag", GL_FRAGMENT_SHADER),
            },
            "MapShader"
        }
    );
}

void Sickle::MapArea::on_unrealize()
{
}

bool Sickle::MapArea::on_render(Glib::RefPtr<Gdk::GLContext> const &context)
{
    throw_if_error();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup projection matrix.
    auto const projectionMatrix = glm::perspective(
        glm::radians(_camera.fov),
        get_width() / (float)get_height(),
        0.1f, 1000.0f);

    auto const modelMatrix = _transform.getMatrix();

    // Draw models.
    _shader->use();
    glActiveTexture(GL_TEXTURE0);
    _shader->setUniformS("view", _camera.getViewMatrix());
    _shader->setUniformS("projection", projectionMatrix);
    _shader->setUniformS("tex", 0);
    _shader->setUniformS("model", modelMatrix);
    _glmap.render();
    return true;
}

bool Sickle::MapArea::on_key_press_event(GdkEventKey *event)
{
    switch (event->keyval)
    {
    case GDK_KEY_z:
    case GDK_KEY_Z:
        make_current();
        _prop_wireframe = !_prop_wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, _prop_wireframe? GL_LINE : GL_FILL);
        queue_render();
        return true;
        break;

    case GDK_KEY_a:
    case GDK_KEY_A:
        _state.move_direction.x = 1.0f;
        break;
    case GDK_KEY_d:
    case GDK_KEY_D:
        _state.move_direction.x = -1.0f;
        break;

    case GDK_KEY_w:
    case GDK_KEY_W:
        _state.move_direction.z = 1.0f;
        break;
    case GDK_KEY_s:
    case GDK_KEY_S:
        _state.move_direction.z = -1.0f;
        break;

    case GDK_KEY_q:
    case GDK_KEY_Q:
        _state.move_direction.y = 1.0f;
        break;
    case GDK_KEY_e:
    case GDK_KEY_E:
        _state.move_direction.y = -1.0f;
        break;

    case GDK_KEY_Shift_L:
    case GDK_KEY_Shift_R:
        _state.gofast = true;
        break;

    default:
        return Gtk::GLArea::on_key_press_event(event);
        break;
    }
    return true;
}

bool Sickle::MapArea::on_key_release_event(GdkEventKey *event)
{
    switch (event->keyval)
    {
    case GDK_KEY_a:
    case GDK_KEY_A:
        _state.move_direction.x = 0.0f;
        break;
    case GDK_KEY_d:
    case GDK_KEY_D:
        _state.move_direction.x = 0.0f;
        break;

    case GDK_KEY_s:
    case GDK_KEY_S:
        _state.move_direction.z = 0.0f;
        break;
    case GDK_KEY_w:
    case GDK_KEY_W:
        _state.move_direction.z = 0.0f;
        break;

    case GDK_KEY_e:
    case GDK_KEY_E:
        _state.move_direction.y = 0.0f;
        break;
    case GDK_KEY_q:
    case GDK_KEY_Q:
        _state.move_direction.y = 0.0f;
        break;

    case GDK_KEY_Shift_L:
    case GDK_KEY_Shift_R:
        _state.gofast = false;
        break;

    default:
        return Gtk::GLArea::on_key_release_event(event);
        break;
    }
    return true;
}


bool Sickle::MapArea::tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock)
{
    static float const USEC_TO_SECONDS = 0.000001;

    auto const frame_time = clock->get_frame_time();
    if (_state.last_frame_time == 0)
        goto done;
    if (glm::length(_state.move_direction) == 0.0f)
        goto done;

    {   // Block needed for gotos to compile
    auto const frame_delta = frame_time - _state.last_frame_time;
    auto const delta = frame_delta * USEC_TO_SECONDS;

    auto const direction = glm::normalize(_state.move_direction);
    auto const motion = direction * _camera.speed * (_state.gofast? _prop_shift_multiplier : 1.0f);
    _camera.translate(motion * delta);
    queue_render();
    }

done:
    _state.last_frame_time = frame_time;
    return G_SOURCE_CONTINUE;
}

bool Sickle::MapArea::on_button_press_event(GdkEventButton *event)
{
    if (event->button == 2)
    {
        _state.pointer_prev_x = event->x;
        _state.pointer_prev_y = event->y;
        return true;
    }
    return Gtk::GLArea::on_button_press_event(event);
}

bool Sickle::MapArea::on_motion_notify_event(GdkEventMotion *event)
{
    if (event->state & Gdk::BUTTON2_MASK)
    {
        auto dx = event->x - _state.pointer_prev_x;
        auto dy = event->y - _state.pointer_prev_y;
        _camera.rotate(glm::vec2{dx, dy} * _prop_mouse_sensitivity);
        _state.pointer_prev_x = event->x;
        _state.pointer_prev_y = event->y;
        queue_render();
        return true;
    }
    return Gtk::GLArea::on_motion_notify_event(event);
}

bool Sickle::MapArea::on_scroll_event(GdkEventScroll *event)
{
    if (event->state & Gdk::MOD1_MASK)
    {
        switch (event->direction)
        {
        case GDK_SCROLL_DOWN:
            _camera.setFOV(_camera.fov + 1.0f);
            break;
        case GDK_SCROLL_UP:
            _camera.setFOV(_camera.fov - 1.0f);
            break;
        }
        queue_render();
        return true;
    }
    return Gtk::GLArea::on_scroll_event(event);
}
