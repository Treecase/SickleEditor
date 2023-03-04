
local function clamp(x, min, max)
    if x < min then return min
    elseif x > max then return max
    else return x end
end

-- Constants for panning/zoom control.
local MOVE_SMOOTH = 8.0
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
    -- Reset zoom with 0.
    elseif keyval == LuaGDK.GDK_KEY_0 then
        transform:set_zoom(1.0)

    -- Hold Ctrl to select multiple brushes.
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state:set_multiselect(true)
        self.ctrl = true
    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        self.shift = true

    -- Pressing escape cancels any current actions.
    elseif keyval == LuaGDK.GDK_KEY_Escape then
        self.dragging_selection = nil

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
        self.ctrl = false

    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        self.shift = false

    else
        return false
    end
    return true
end


-- Mouse button pressed.
function gAppWin.topMapArea:on_button_press_event(event)
    local state = self:get_state()
    local gbox = self:get_selection_box()

    if event.button == 1 then
        local hovered = gbox:check_point(self:screenspace_to_drawspace(event.x, event.y))
        -- Clicking inside the selection box begins a selection drag.
        if hovered == grabbablebox.BOX then
            local drag = {}
            drag.x = event.x
            drag.y = event.y
            drag.moved = false
            drag.accum = geo.vector.new()
            self.dragging_selection = drag

        -- Left click can either be the start of a brushbox draw, or just a
        -- simple click. Either way, we set the brushbox to be 0 width on the
        -- click, and clear the state's `dragged` property.
        else
            local editor = self:get_editor()
            local x,y,z = self:drawspace_to_worldspace(self:screenspace_to_drawspace(event.x, event.y))
            editor:get_brushbox():set_start(x, y, z)
            editor:get_brushbox():set_end(x, y, z)
            state:set_dragged(false)
        end

    -- Middle click begins view panning.
    elseif event.button == 2 then
        state:set_pointer_prev(event)

    else
        return false
    end

    self:set_state(state)
    return true
end


-- Mouse button released.
function gAppWin.topMapArea:on_button_release_event(event)
    local state = self:get_state()
    local editor = self:get_editor()

    -- Stop dragging selection.
    if self.dragging_selection then
        -- A selection drag occurred. We just have to clean up.
        if self.dragging_selection.moved then
            self.dragging_selection = nil
            return true

        -- If we clicked within the selection, but didn't drag, we want to
        -- deselect the clicked brush.
        else
            self.dragging_selection = nil
            -- vvv Brush deselection done below. vvv
        end
    end

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
    local ret = false

    local editor = self:get_editor()
    local state = self:get_state()
    local transform = self:get_transform()

    -- Change cursor if we're hovering a GrabBox selection point.
    local gbox = self:get_selection_box()
    local hovered = gbox:check_point(self:screenspace_to_drawspace(event.x, event.y))

    local CURSORS = {
        [grabbablebox.BOX] = "crosshair",
        [grabbablebox.NE] = "ne-resize",
        [grabbablebox.NW] = "nw-resize",
        [grabbablebox.SE] = "se-resize",
        [grabbablebox.SW] = "sw-resize",
        [grabbablebox.N] = "n-resize",
        [grabbablebox.E] = "e-resize",
        [grabbablebox.S] = "s-resize",
        [grabbablebox.W] = "w-resize"
    }
    local cursor = CURSORS[hovered] or "default"

    -- Selection Drag.
    if self.dragging_selection then
        local curr = geo.vector.new(self:drawspace_to_worldspace(self:screenspace_to_drawspace(event.x, event.y)))
        local prev = geo.vector.new(self:drawspace_to_worldspace(self:screenspace_to_drawspace(self.dragging_selection.x, self.dragging_selection.y)))

        local delta = curr - prev
        local accum = self.dragging_selection.accum + delta

        local grid = gAppWin:get_grid_size()
        local rounded = geo.vector.map(math.floor, accum / grid) * grid
        local rounded_prev = geo.vector.map(math.floor, self.dragging_selection.accum / grid) * grid

        local selection = self:get_editor():get_selection()
        for brush in selection:iterate() do
            brush:translate(rounded - rounded_prev)
        end

        self.dragging_selection.x = event.x
        self.dragging_selection.y = event.y
        self.dragging_selection.moved = true
        self.dragging_selection.accum = accum
        ret = true

    else
        -- Set selection box when dragging with left-click.
        if event.state & LuaGDK.GDK_BUTTON1_MASK ~= 0 then
            editor:get_brushbox():set_end(self:drawspace_to_worldspace(self:screenspace_to_drawspace(event.x, event.y)))
            state:set_dragged(true)
            editor:get_selection():clear()
            ret = true

        -- Pan view when dragging with middle-click.
        elseif event.state & LuaGDK.GDK_BUTTON2_MASK ~= 0 then
            transform:set_x(transform:get_x() + event.x - state:get_pointer_prev().x)
            transform:set_y(transform:get_y() + event.y - state:get_pointer_prev().y)
            state:set_pointer_prev(event)
            ret = true
        end
    end

    self:set_cursor(cursor)
    self:set_state(state)
    self:set_transform(transform)
    return ret
end


-- Mouse scrolled on widget.
function gAppWin.topMapArea:on_scroll_event(event)
    local transform = self:get_transform()

    -- shift+ctrl+scroll = scroll vertical
    if self.shift and self.ctrl then
        if event.direction == LuaGDK.GDK_SCROLL_DOWN then
            transform:set_y(transform:get_y() - MOVE_SMOOTH)
        elseif event.direction == LuaGDK.GDK_SCROLL_UP then
            transform:set_y(transform:get_y() + MOVE_SMOOTH)
        end

    -- shift+scroll = scroll horizontal
    elseif self.shift and not self.ctrl then
        if event.direction == LuaGDK.GDK_SCROLL_DOWN then
            transform:set_x(transform:get_x() - MOVE_SMOOTH)
        elseif event.direction == LuaGDK.GDK_SCROLL_UP then
            transform:set_x(transform:get_x() + MOVE_SMOOTH)
        end

    -- scroll horizontal
    elseif event.direction == LuaGDK.GDK_SCROLL_LEFT then
        transform:set_x(transform:get_x() + MOVE_SMOOTH)
    elseif event.direction == LuaGDK.GDK_SCROLL_RIGHT then
        transform:set_x(transform:get_x() - MOVE_SMOOTH)

    -- scroll = zoom in/out
    else
        if event.direction == LuaGDK.GDK_SCROLL_DOWN then
            transform:set_zoom(clamp(transform:get_zoom() / ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
        elseif event.direction == LuaGDK.GDK_SCROLL_UP then
            transform:set_zoom(clamp(transform:get_zoom() * ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
        end
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
