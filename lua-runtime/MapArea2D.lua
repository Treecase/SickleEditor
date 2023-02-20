
local function clamp(x, min, max)
    if x < min then return min
    elseif x > max then return max
    else return x end
end

-- Constants for panning/zoom control.
local MOVE_STEP = 64.0
local ZOOM_MULTIPLIER_SMOOTH = 1.1
local ZOOM_MULTIPLIER_STEP = 2.0
local MIN_ZOOM = 1.0 / 16.0
local MAX_ZOOM = 16.0


-- Keyboard key pressed.
function gAppWin.topMapArea:on_key_press_event(keyval)
    local transform = self:get_transform()
    local state = self:get_state()

    -- Pan view using arrow keys.
    if keyval == LuaGDK.GDK_KEY_Up then
        transform:set_y(transform:get_y() + MOVE_STEP)
    elseif keyval == LuaGDK.GDK_KEY_Down then
        transform:set_y(transform:get_y() - MOVE_STEP)
    elseif keyval == LuaGDK.GDK_KEY_Left then
        transform:set_x(transform:get_x() + MOVE_STEP)
    elseif keyval == LuaGDK.GDK_KEY_Right then
        transform:set_x(transform:get_x() - MOVE_STEP)

    -- Zoom in/out with keypad +/-.
    elseif keyval == LuaGDK.GDK_KEY_KP_Add then
        transform:set_zoom(clamp(transform:get_zoom() * ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM))
    elseif keyval == LuaGDK.GDK_KEY_KP_Subtract then
        transform:set_zoom(clamp(transform:get_zoom() / ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM))

    -- Hold Ctrl to select multiple brushes.
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state:set_multiselect(true)

    else
        return false
    end

    self:set_transform(transform)
    self:set_state(state)
    return true
end


-- Keyboard key released.
function gAppWin.topMapArea:on_key_release_event(keyval)
    -- Turn off multi-brush selecting when Ctrl is released.
    if keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        local state = self:get_state()
        state:set_multiselect(false)
        self:set_state(state)
        return true
    end
    return false
end


-- Mouse button pressed.
function gAppWin.topMapArea:on_button_press_event(event)
    local state = self:get_state()
    -- Left click can either be the start of a selection drag, or just a simple
    -- click. Either way, we set the brushbox to be 0 width on the click, and
    -- clear the state's `dragged` property.
    if event.button == 1 then
        local editor = self:get_editor()
        local x,y,z = self:drawspace_to_worldspace(self:screenspace_to_drawspace(event.x, event.y))
        editor:get_brushbox():set_start(x, y, z)
        editor:get_brushbox():set_end(x, y, z)
        state:set_dragged(false)
        self:set_state(state)
        return true
    end
    -- Middle click begins view panning.
    if event.button == 2 then
        state:set_pointer_prev(event)
        self:set_state(state)
        return true
    end
    return false
end


-- Mouse button released.
function gAppWin.topMapArea:on_button_release_event(event)
    local state = self:get_state()
    local editor = self:get_editor()
    -- Select a brush on left click.
    if event.button == 1 and not state:get_dragged() then
        -- Clear selection if we're only picking one at a time.
        if not state:get_multiselect() then
            editor:get_selection():clear()
        end
        -- Pick a brush based on the click position.
        local x,y = self:screenspace_to_drawspace(event.x, event.y)
        local picked = self:pick_brush(x, y)
        if picked then
            if picked:is_selected() then
                editor:get_selection():remove(picked)
            else
                editor:get_selection():add(picked)
            end
        end
        self:set_state(state)
        return true
    end
    return false
end


-- Mouse moved over widget.
function gAppWin.topMapArea:on_motion_notify_event(event)
    local transform = self:get_transform()
    local state = self:get_state()
    local editor = self:get_editor()

    -- Set selection box when dragging with left-click.
    if event.state & LuaGDK.GDK_BUTTON1_MASK ~= 0 then
        editor:get_brushbox():set_end(self:drawspace_to_worldspace(self:screenspace_to_drawspace(event.x, event.y)))
        state:set_dragged(true)
        editor:get_selection():clear()
        self:set_state(state)
        return true

    -- Pan view when dragging with middle-click.
    elseif event.state & LuaGDK.GDK_BUTTON2_MASK ~= 0 then
        transform:set_x(transform:get_x() + event.x - state:get_pointer_prev().x)
        transform:set_y(transform:get_y() + event.y - state:get_pointer_prev().y)
        state:set_pointer_prev(event)
        self:set_transform(transform)
        self:set_state(state)
        return true
    end

    return false
end


-- Mouse scrolled on widget.
function gAppWin.topMapArea:on_scroll_event(event)
    local transform = self:get_transform()

    -- Zoom in/out with scrollwheel.
    if event.direction == LuaGDK.GDK_SCROLL_DOWN then
        transform:set_zoom(clamp(transform:get_zoom() / ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
    elseif event.direction == LuaGDK.GDK_SCROLL_UP then
        transform:set_zoom(clamp(transform:get_zoom() * ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
    end

    self:set_transform(transform)
    return true
end


-- All MapAreas should act the same.
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
