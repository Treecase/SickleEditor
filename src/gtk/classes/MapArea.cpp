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


#define DEFAULT_MOUSE_SENSITIVITY   0.75f
#define DEFAULT_MOVE_SPEED  1.0f
#define DEFAULT_FOV 70.0f   // degrees
#define MIN_FOV     30.0f   // degrees
#define MAX_FOV     90.0f   // degrees

#define TURN_RATE   120.0f  // degrees/second
#define FOV_DELTA   1.0f    // degrees

#define NEAR_PLANE  0.1f
#define FAR_PLANE   1000.0f


char const *const Sickle::MapArea::Debug::rayShaderVertexSource{
"#version 430 core\n"
"layout(location=0) in vec3 vPos;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()"
"{"
"    gl_Position = projection * view * vec4(vPos, 1.0);"
"}"
};
char const *const Sickle::MapArea::Debug::rayShaderFragmentSource{
"#version 430 core\n"
"out vec4 FragColor;"
"uniform vec3 color;"
"void main()"
"{"
"    FragColor = vec4(color, 1);"
"}"
};

/* ===[ MapArea ]=== */
Sickle::MapArea::MapArea(Editor &ed)
:   Glib::ObjectBase{typeid(MapArea)}
,   Gtk::GLArea{}
,   _editor{ed}
,   _shader{nullptr}
,   _camera{
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f},
        DEFAULT_FOV,
        DEFAULT_MOVE_SPEED,
        MIN_FOV, MAX_FOV}
,   _transform{
        {0.0f, 0.0f, 0.0f},
        {glm::radians(-90.0f), 0.0f, 0.0f},
        {0.005f, 0.005f, 0.005f}}
,   _state{
        0.0, 0.0,
        0,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f},
        false
    }
,   _prop_wireframe{false}
,   _prop_shift_multiplier{2.0f}
,   _prop_mouse_sensitivity{DEFAULT_MOUSE_SENSITIVITY}
{
    set_required_version(4, 3);
    set_use_es(false);
    set_has_depth_buffer(true);
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_auto_render(true);
    set_can_focus(true);

    _editor.signal_map_changed().connect(
        sigc::mem_fun(*this, &MapArea::on_editor_map_changed));
    _editor.selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea::queue_render));
    _editor.brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea::queue_render));

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK
        | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK
        | Gdk::SCROLL_MASK
        | Gdk::ENTER_NOTIFY_MASK);

    add_tick_callback(sigc::mem_fun(*this, &MapArea::tick_callback));
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

    debug.init();
    _synchronize_glmap();
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
        NEAR_PLANE, FAR_PLANE);

    auto const modelMatrix = _transform.getMatrix();

    // Draw models.
    _shader->use();
    glActiveTexture(GL_TEXTURE0);
    _shader->setUniformS("view", _camera.getViewMatrix());
    _shader->setUniformS("projection", projectionMatrix);
    _shader->setUniformS("tex", 0);
    _shader->setUniformS("model", modelMatrix);
    // TODO: Do this properly, very brittle right now
    for (size_t e = 0; e < _mapview->entities.size(); ++e)
    {
        for (size_t b = 0; b < _mapview->entities[e].brushes.size(); ++b)
        {
            if (_editor.get_map().entities[e].brushes[b].is_selected)
                _shader->setUniformS("modulate", glm::vec3{1, 0, 0});
            else
                _shader->setUniformS("modulate", glm::vec3{1, 1, 1});
            _mapview->entities[e].brushes[b].render();
        }
    }

    debug.drawRay(_camera.getViewMatrix(), projectionMatrix);

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

    case GDK_KEY_Up:
        _state.turn_rates.y = -TURN_RATE;
        break;
    case GDK_KEY_Down:
        _state.turn_rates.y = TURN_RATE;
        break;
    case GDK_KEY_Left:
        _state.turn_rates.x = -TURN_RATE;
        break;
    case GDK_KEY_Right:
        _state.turn_rates.x = TURN_RATE;
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

    case GDK_KEY_Up:
    case GDK_KEY_Down:
        _state.turn_rates.y = 0.0f;
        break;
    case GDK_KEY_Left:
    case GDK_KEY_Right:
        _state.turn_rates.x = 0.0f;
        break;

    default:
        return Gtk::GLArea::on_key_release_event(event);
        break;
    }
    return true;
}


bool Sickle::MapArea::tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock)
{
    static constexpr float const USEC_TO_SECONDS = 0.000001f;

    auto const frame_time = clock->get_frame_time();
    auto const frame_delta = frame_time - _state.last_frame_time;
    auto const delta = frame_delta * USEC_TO_SECONDS;
    _state.last_frame_time = frame_time;

    if (glm::length(_state.move_direction) != 0.0f)
    {
        auto const direction = glm::normalize(_state.move_direction);
        auto const motion = direction * _camera.speed * (_state.gofast? _prop_shift_multiplier : 1.0f);
        _camera.translate(motion * delta);
        queue_render();
    }

    if (glm::length(_state.turn_rates) != 0.0f)
    {
        _camera.rotate(_state.turn_rates * (_state.gofast? _prop_shift_multiplier : 1.0f) * _prop_mouse_sensitivity * delta);
        queue_render();
    }

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

bool Sickle::MapArea::on_button_release_event(GdkEventButton *event)
{
    if (event->button == 1)
    {
        if (!_editor.get_map().entities.empty())
        {
            auto picked = pick_brush({event->x, event->y});
            if (picked)
            {
                if (picked->is_selected)
                    _editor.selected.remove(picked);
                else
                    _editor.selected.add(picked);
            }
        }
        return true;
    }
    return Gtk::GLArea::on_button_release_event(event);
}

bool Sickle::MapArea::on_enter_notify_event(GdkEventCrossing *event)
{
    grab_focus();
    return true;
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
            _camera.setFOV(_camera.fov + FOV_DELTA);
            break;
        case GDK_SCROLL_UP:
            _camera.setFOV(_camera.fov - FOV_DELTA);
            break;
        }
        queue_render();
        return true;
    }
    return Gtk::GLArea::on_scroll_event(event);
}

void Sickle::MapArea::on_editor_map_changed()
{
    if (get_realized())
    {
        _synchronize_glmap();
        queue_render();
    }
}


void Sickle::MapArea::_synchronize_glmap()
{
    make_current();
    _mapview.reset(new MAP::GLMap{_editor.get_map()});
    queue_render();
}


/** Templated bounding box using glm vectors. */
template<glm::length_t L, typename T>
struct BBox
{
    using Point = glm::vec<L, T>;
    Point min{INFINITY}, max{-INFINITY};
    T volume() const {
        auto const wh = glm::abs(max - min);
        return wh.x * wh.y;
    }
    bool contains(Point point) const {
        return (
            glm::all(glm::lessThanEqual(min, point))
            && glm::all(glm::lessThanEqual(point, max))
        );
    }
    void add(Point pt) {
        for (typename Point::length_type i = 0; i < Point::length(); ++i)
        {
            if (pt[i] < min[i]) min[i] = pt[i];
            if (pt[i] > max[i]) max[i] = pt[i];
        }
    }
};

bool raycast(glm::vec3 pos, glm::vec3 delta, BBox<3, float> const &bbox, float &t)
{
    // https://people.csail.mit.edu/amy/papers/box-jgt.pdf
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    if (delta.x >= 0)
    {
        tmin = (bbox.min.x - pos.x) / delta.x;
        tmax = (bbox.max.x - pos.x) / delta.x;
    }
    else
    {
        tmin = (bbox.max.x - pos.x) / delta.x;
        tmax = (bbox.min.x - pos.x) / delta.x;
    }
    if (delta.y >= 0)
    {
        tymin = (bbox.min.y - pos.y) / delta.y;
        tymax = (bbox.max.y - pos.y) / delta.y;
    }
    else
    {
        tymin = (bbox.max.y - pos.y) / delta.y;
        tymax = (bbox.min.y - pos.y) / delta.y;
    }
    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    if (delta.z >= 0)
    {
        tzmin = (bbox.min.z - pos.z) / delta.z;
        tzmax = (bbox.max.z - pos.z) / delta.z;
    }
    else
    {
        tzmin = (bbox.max.z - pos.z) / delta.z;
        tzmax = (bbox.min.z - pos.z) / delta.z;
    }
    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    t = glm::min(tmin, tmax);
    return (tmin < INFINITY) && (tmax > 0);
}

Sickle::MapArea::GLSpacePoint
Sickle::MapArea::screenspace_to_glspace(ScreenSpacePoint const &point) const
{
    return {
        point.x - 0.5*get_allocated_width(),
        -(point.y - 0.5*get_allocated_height()),
        0
    };
}

Sickle::EditorBrush *Sickle::MapArea::pick_brush(glm::vec2 const &ssp)
{
    using BBox = BBox<MAP::Vertex::length(), MAP::Vertex::value_type>;

    EditorBrush *picked{nullptr};
    float pt = INFINITY;

    // TODO: For now we pick straight forward from the camera, without
    // considering where the user actually clicked
    auto const ray_delta = glm::normalize(_camera.getLookDirection());

    // Camera is operating in GL space, map vertices are in map space. This is
    // used to transform map vertices into GL space.
    auto const modelview = _transform.getMatrix();

    for (auto const &entity : _editor.get_map().entities)
    {
        for (auto const &brush : entity.brushes)
        {
            BBox bbox{};
            for (auto const &face : brush.planes)
                for (auto const &vertex : face.vertices)
                    bbox.add(glm::vec3{modelview * glm::vec4{vertex, 1.0f}});

            float t;
            if (raycast(_camera.pos, ray_delta, bbox, t))
            {
                // We pick the first (ie. closest) brush our raycast hits.
                if (t < pt)
                {
                    picked = const_cast<EditorBrush *>(&brush);
                    pt = t;
                }
            }
        }
    }
    debug.setRayPoints(_camera.pos, _camera.pos + ray_delta * pt);
    return picked;
}
