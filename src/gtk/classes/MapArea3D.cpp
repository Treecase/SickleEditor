/**
 * MapArea3D.cpp - Sickle editor main window GLArea.
 * Copyright (C) 2022-2024 Trevor Last
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

#include "MapArea3D.hpp"

#include <gtkmm/messagedialog.h>
#include <utils/BoundingBox.hpp>
#include <world3d/RenderComponentFactory.hpp>
#include <world3d/raycast/Collider.hpp>
#include <world3d/raycast/ColliderFactory.hpp>

#include <iostream>


#define DEFAULT_MOUSE_SENSITIVITY   0.75f

#define NEAR_PLANE  0.1f
#define FAR_PLANE   1000.0f

#define DEFAULT_CAMERA  {{0.0f, 0.0f, 0.0f}, {glm::radians(180.0f), 0.0f}, 70.0f, 1.0f, 30.0f, 90.0f}
#define DEFAULT_TRANSFORM   {{0.0f, 0.0f, 0.0f}, {glm::radians(-90.0f), 0.0f, 0.0f}, {0.005f, 0.005f, 0.005f}}


bool
raycast(glm::vec3 pos, glm::vec3 delta, BBox3 const &bbox, float &t)
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



/* ===[ MapArea3D ]=== */
Sickle::MapArea3D::MapArea3D(Editor::EditorRef ed)
:   Glib::ObjectBase{typeid(MapArea3D)}
,   Gtk::GLArea{}
,   Lua::Referenceable{}
,   _editor{ed}
,   _prop_camera{*this, "camera", DEFAULT_CAMERA}
,   _prop_state{*this, "state", {}}
,   _prop_transform{*this, "transform", DEFAULT_TRANSFORM}
,   _prop_wireframe{*this, "wireframe", false}
,   _prop_shift_multiplier{*this, "grid-size", 2.0f}
,   _prop_mouse_sensitivity{
        *this, "mouse-sensitivity", DEFAULT_MOUSE_SENSITIVITY}
{
    set_required_version(4, 3);
    set_use_es(false);
    set_has_depth_buffer(true);
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_auto_render(true);
    set_can_focus(true);

    _editor->property_map().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea3D::on_editor_map_changed));
    _editor->selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea3D::queue_render));
    _editor->brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea3D::queue_render));

    World3D::Face::signal_missing_texture().connect(
        sigc::mem_fun(*this, &MapArea3D::on_world3d_face_missing_texture));

    property_transform().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea3D::queue_render));
    property_camera().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea3D::queue_render));
    property_wireframe().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea3D::queue_render));
    property_wireframe().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea3D::on_wireframe_changed));

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK
        | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK
        | Gdk::SCROLL_MASK
        | Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);

    add_tick_callback(sigc::mem_fun(*this, &MapArea3D::tick_callback));


    // Set global PointEntityBox 3D render callback.
    World3D::PointEntityBox::predraw =\
        [this](
            World3D::PointEntityBox::ShaderParams &params,
            Editor::Entity const *entity) -> void
        {
            auto const &camera = property_camera().get_value();
            params.model =\
                property_transform().get_value().getMatrix() * params.model;
            params.view = camera.getViewMatrix();
            params.projection = glm::perspective(
                glm::radians(camera.fov),
                get_width() / (float)get_height(),
                NEAR_PLANE, FAR_PLANE);
        };

    // Set global PointEntitySprite 3D render callback.
    World3D::PointEntitySprite::predraw =\
        [this](
            World3D::PointEntitySprite::ShaderParams &params,
            Editor::Entity const *entity) -> void
        {
            auto const &camera = property_camera().get_value();
            params.model =\
                property_transform().get_value().getMatrix() * params.model;
            params.view = camera.getViewMatrix();
            params.projection = glm::perspective(
                glm::radians(camera.fov),
                get_width() / (float)get_height(),
                NEAR_PLANE, FAR_PLANE);
        };

    // Set global Brush 3D render callback.
    World3D::Brush::predraw =\
        [this](GLUtil::Program &shader, Editor::Brush const *brush) -> void {
            auto const &camera = property_camera().get_value();
            shader.setUniformS(
                "model",
                property_transform().get_value().getMatrix());
            shader.setUniformS("view", camera.getViewMatrix());
            shader.setUniformS(
                "projection",
                glm::perspective(
                    glm::radians(camera.fov),
                    get_width() / (float)get_height(),
                    NEAR_PLANE, FAR_PLANE));
        };

    // Set global Face 3D render callback.
    World3D::Face::predraw =\
        [this](GLUtil::Program &shader, Editor::Face const *face) -> void {
        };
}


Sickle::Editor::EditorObjectRef
Sickle::MapArea3D::pick_object(glm::vec2 const &ssp)
{
    static auto const is_collider =\
        [](std::shared_ptr<Component> const &c) -> bool {
            if (std::dynamic_pointer_cast<World3D::BoxCollider>(c))
                return true;
            return false;
        };

    Sickle::Editor::EditorObjectRef picked{nullptr};
    float pt = INFINITY;

    auto const &_camera = property_camera().get_value();
    // TODO: For now we pick straight forward from the camera, without
    // considering where the user actually clicked
    auto const ray_delta = glm::normalize(_camera.getLookDirection());

    // Camera is operating in GL space, map vertices are in map space. This is
    // used to transform map vertices into GL space.
    auto const modelview = property_transform().get_value().getMatrix();

    auto const objects = _editor->get_map()->children_recursive_breadth_first();
    for (auto const &obj : objects)
    {
        auto const colliders = obj->get_components_matching(is_collider);
        for (auto const &collider : colliders)
        {
            auto const c = std::dynamic_pointer_cast<World3D::Collider>(
                collider);
            auto const bbox = c->get_box();

            BBox3 const bbox_transformed{
                modelview * glm::vec4{bbox.min, 1.0f},
                modelview * glm::vec4{bbox.max, 1.0f}};

            float t;
            if (raycast(_camera.pos, ray_delta, bbox_transformed, t))
            {
                // We pick the first (ie. closest) brush our raycast hits.
                if (t < pt)
                {
                    picked = obj;
                    pt = t;
                }
            }
        }
    }

    debug.setRayPoints(_camera.pos, _camera.pos + ray_delta * pt);
    return picked;
}


Sickle::MapArea3D::GLSpacePoint
Sickle::MapArea3D::screenspace_to_glspace(ScreenSpacePoint const &point) const
{
    return {
        point.x - 0.5*get_allocated_width(),
        -(point.y - 0.5*get_allocated_height()),
        0
    };
}


void Sickle::MapArea3D::on_realize()
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

    debug.init();
    _synchronize_glmap();
}


void Sickle::MapArea3D::on_unrealize()
{
}


bool Sickle::MapArea3D::on_render(Glib::RefPtr<Gdk::GLContext> const &context)
{
    auto const &camera = property_camera().get_value();

    throw_if_error();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Let deferred functions run.
    DeferredExec::context_ready();

    // Draw the world.
    // Walk the world tree and execute any World3D render components.
    // Note that the traversal must be done in depth-first ordering, to allow
    // parents to set things up for their children.
    static auto const is_render_component =\
        [](std::shared_ptr<Component> const &c) -> bool {
            if (std::dynamic_pointer_cast<World3D::RenderComponent>(c))
                return true;
            return false;
        };
    static auto const execute_render_components =\
        [](Editor::EditorObjectRef const &obj) -> void {
            auto const &components =\
                obj->get_components_matching(is_render_component);
            for (auto const &rc : components)
                rc->execute();
        };
    _editor->get_map()->foreach(execute_render_components);

    // Stop deferred functions from running.
    DeferredExec::context_unready();

    // Draw debugging ray.
    auto const projectionMatrix = glm::perspective(
        glm::radians(camera.fov),
        get_width() / (float)get_height(),
        NEAR_PLANE, FAR_PLANE);
    debug.drawRay(camera.getViewMatrix(), projectionMatrix);

    return true;
}



bool Sickle::MapArea3D::tick_callback(
    Glib::RefPtr<Gdk::FrameClock> const &clock)
{
    static constexpr float const USEC_TO_SECONDS = 0.000001f;

    auto _camera = property_camera().get_value();
    auto _state = property_state().get_value();

    auto const frame_time = clock->get_frame_time();
    auto const frame_delta = frame_time - _state.last_frame_time;
    auto const delta = frame_delta * USEC_TO_SECONDS;
    _state.last_frame_time = frame_time;

    if (glm::length(_state.move_direction) != 0.0f)
    {
        auto const direction = glm::normalize(_state.move_direction);
        auto const motion = (
            direction
            * _camera.speed
            * (_state.gofast? property_shift_multiplier().get_value() : 1.0f));
        _camera.translate(motion * delta);
    }

    if (glm::length(_state.turn_rates) != 0.0f)
    {
        _camera.rotate(
            _state.turn_rates
            * (_state.gofast? property_shift_multiplier().get_value() : 1.0f)
            * property_mouse_sensitivity().get_value() * delta);
    }

    property_camera().set_value(_camera);
    property_state().set_value(_state);
    return G_SOURCE_CONTINUE;
}


bool Sickle::MapArea3D::on_enter_notify_event(GdkEventCrossing *event)
{
    grab_focus();
    return true;

}


bool Sickle::MapArea3D::on_leave_notify_event(GdkEventCrossing *crossing_event)
{
    auto state = property_state().get_value();
    state.move_direction = glm::vec3{0, 0, 0};
    property_state().set_value(state);
    return true;
}


void Sickle::MapArea3D::on_editor_map_changed()
{
    property_state().reset_value();
    property_camera().set_value(DEFAULT_CAMERA);
    property_transform().set_value(DEFAULT_TRANSFORM);
    if (get_realized())
    {
        _synchronize_glmap();
        debug.setRayPoints({0,0,0}, {0,0,0});
        queue_render();
    }
}


void Sickle::MapArea3D::on_wireframe_changed()
{
    make_current();
    glPolygonMode(
        GL_FRONT_AND_BACK,
        property_wireframe().get_value()? GL_LINE : GL_FILL);
    if (property_wireframe().get_value())
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
}


void Sickle::MapArea3D::on_world3d_face_missing_texture(std::string const &what)
{
    _error_tracker.missing_textures.insert(what);
}



void Sickle::MapArea3D::_check_errors()
{
    if (_error_tracker.error_occurred())
    {
        std::string msg = "<big><b>World3D Error(s):</b></big>";
        if (!_error_tracker.missing_textures.empty())
        {
            msg += "\nMissing textures:";
            for (auto const &texture : _error_tracker.missing_textures)
                msg += "\n<small>" + texture + "</small>";
        }
        Gtk::MessageDialog d{msg, true, Gtk::MessageType::MESSAGE_WARNING};
        d.set_title("World3D Error(s)");
        d.run();
    }
}


void Sickle::MapArea3D::_synchronize_glmap()
{
    static auto const add_brush = [](Editor::EditorObjectRef child) -> void {
        auto const brush = Editor::BrushRef::cast_dynamic(child);
        brush->add_component(
            World3D::RenderComponentFactory{}.construct(brush));
        brush->add_component(World3D::ColliderFactory{}.construct(brush));
    };

    static auto const add_entity = [](Editor::EditorObjectRef child) -> void {
        auto const entity = Editor::EntityRef::cast_dynamic(child);
        entity->add_component(
            World3D::RenderComponentFactory{}.construct(entity));
        entity->add_component(World3D::ColliderFactory{}.construct(entity));

        sigc::connection conn = entity->signal_child_added().connect(add_brush);
        entity->signal_removed().connect(
            [conn]() mutable -> void {conn.disconnect();});
        entity->foreach_direct(add_brush);
    };


    make_current();
    _error_tracker = ErrorTracker{};

    auto const world = _editor->get_map();
    sigc::connection conn = world->signal_child_added().connect(add_entity);
    world->signal_removed().connect(
        [conn]() mutable -> void {conn.disconnect();});
    world->foreach_direct(add_entity);

    _check_errors();
    queue_render();
}
