
local function clamp(x, min, max)
    if x < min then return min
    elseif x > max then return max
    else return x end
end

local MOVE_STEP = 64.0
local ZOOM_MULTIPLIER_SMOOTH = 1.1
local ZOOM_MULTIPLIER_STEP = 2.0
local MIN_ZOOM = 1.0 / 16.0
local MAX_ZOOM = 16.0


function gAppWin.topMapArea:on_key_press_event(keyval)
    local transform = self:get_transform()
    local state = self:get_state()

    if keyval == LuaGDK.GDK_KEY_Up then
        transform.y = transform.y + MOVE_STEP
    elseif keyval == LuaGDK.GDK_KEY_Down then
        transform.y = transform.y - MOVE_STEP
    elseif keyval == LuaGDK.GDK_KEY_Left then
        transform.x = transform.x + MOVE_STEP
    elseif keyval == LuaGDK.GDK_KEY_Right then
        transform.x = transform.x - MOVE_STEP

    elseif keyval == LuaGDK.GDK_KEY_KP_Add then
        transform.zoom = clamp(transform.zoom * ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM)
    elseif keyval == LuaGDK.GDK_KEY_KP_Subtract then
        transform.zoom = clamp(transform.zoom / ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM)

    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state.multiselect = true

    else
        return false
    end

    self:set_transform(transform)
    self:set_state(state)
    return true
end


function gAppWin.topMapArea:on_key_release_event(keyval)
    local state = self:get_state()
    if keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state.multiselect = false
    else
        return false
    end
    self:set_state(state)
    return true
end


function gAppWin.topMapArea:on_button_press_event(event)
    local state = self:get_state()
    if event.button == 1 then
        -- auto const v = _drawspace_to_worldspace(_screenspace_to_drawspace(event->x, event->y));
        -- _editor.brushbox.p1(v)
        -- _editor.brushbox.p2(v)
        state.dragged = false
        self:set_state(state)
        return true
    end
    if event.button == 2 then
        state.pointer_prev.x = event.x
        state.pointer_prev.y = event.y
        self:set_state(state)
        return true
    end
    return false
end


function gAppWin.topMapArea:on_button_release_event(event)
  -- auto const &_state = property_state().get_value();
  -- if (event->button == 1 && !_state.dragged)
  -- {
  --   if (!_state.multiselect)
  --     _editor.selected.clear();
  --   if (!_editor.get_map().entities.empty())
  --   {
  --     auto point = _screenspace_to_drawspace(event->x, event->y);
  --     auto picked = pick_brush(point);
  --     if (picked)
  --     {
  --       if (picked->is_selected)
  --         _editor.selected.remove(picked);
  --       else
  --         _editor.selected.add(picked);
  --     }
  --   }
  --   return true;
  -- }
  -- return Gtk::DrawingArea::on_button_release_event(event);
  return false
end


function gAppWin.topMapArea:on_motion_notify_event(event)
    -- auto _transform = property_transform().get_value();
    -- auto _state = property_state().get_value();
    -- if (event->state & Gdk::BUTTON1_MASK)
    -- {
    --     _editor.brushbox.p2(_drawspace_to_worldspace(_screenspace_to_drawspace(event->x, event->y)));
    --     _state.dragged = true;
    --     _editor.selected.clear();
    --     property_state().set_value(_state);
    --     return true;
    -- }
    -- if (event->state & Gdk::BUTTON2_MASK)
    -- {
    --     auto dx = event->x - _state.pointer_prev.x;
    --     auto dy = event->y - _state.pointer_prev.y;
    --     _transform.x += dx;
    --     _transform.y += dy;
    --     _state.pointer_prev.x = event->x;
    --     _state.pointer_prev.y = event->y;
    --     property_transform().set_value(_transform);
    --     property_state().set_value(_state);
    --     return true;
    -- }
    -- return Gtk::DrawingArea::on_motion_notify_event(event);
    return false
end


function gAppWin.topMapArea:on_scroll_event(event)
    local transform = self:get_transform()

    -- GDK_SCROLL_DOWN=1
    if event.direction == 1 then
        transform.zoom = clamp(transform.zoom / ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM)

    -- GDK_SCROLL_UP=0
    elseif event.direction == 0 then
        transform.zoom = clamp(transform.zoom * ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM)
    end

    self:set_transform(transform)
    return true
end


gAppWin.frontMapArea.on_key_press_event = gAppWin.topMapArea.on_key_press_event
gAppWin.frontMapArea.on_key_release_event = gAppWin.topMapArea.on_key_release_event
gAppWin.frontMapArea.on_button_press_event = gAppWin.topMapArea.on_button_press_event
gAppWin.frontMapArea.on_button_release_event = gAppWin.topMapArea.on_button_release_event
gAppWin.frontMapArea.on_motion_notify_event = gAppWin.topMapArea.on_motion_notify_event
gAppWin.frontMapArea.on_scroll_event = gAppWin.topMapArea.on_scroll_event

gAppWin.rightMapArea.on_key_press_event = gAppWin.topMapArea.on_key_press_event
gAppWin.rightMapArea.on_key_release_event = gAppWin.topMapArea.on_key_release_event
gAppWin.rightMapArea.on_button_press_event = gAppWin.topMapArea.on_button_press_event
gAppWin.rightMapArea.on_button_release_event = gAppWin.topMapArea.on_button_release_event
gAppWin.rightMapArea.on_motion_notify_event = gAppWin.topMapArea.on_motion_notify_event
gAppWin.rightMapArea.on_scroll_event = gAppWin.topMapArea.on_scroll_event
