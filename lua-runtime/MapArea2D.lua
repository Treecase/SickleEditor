
local moremath = require "moremath"
local BrushBoxDrag = require "MapArea2D/BrushBoxDrag"
local MoveSelected = require "MapArea2D/MoveSelected"
local ScaleDrag = require "MapArea2D/ScaleDrag"


-- Constants for panning/zoom control.
local MOVE_SMOOTH = 8.0
local MOVE_STEP = 64.0
local ZOOM_MULTIPLIER_SMOOTH = 1.1
local ZOOM_MULTIPLIER_STEP = 2.0
local MIN_ZOOM = 1.0 / 16.0
local MAX_ZOOM = 16.0


function maparea2d.metatable:addListener(listener)
    table.insert(self.listeners, listener)
end

function maparea2d.metatable:removeListener(listener)
    for i,l in ipairs(self.listeners) do
        if l == listener then
            table.remove(self.listeners, i)
        end
    end
end

function maparea2d.metatable:clearListeners()
    for i,_ in ipairs(self.listeners) do
        table.remove(self.listeners, i)
    end
end


-- Mouse button pressed.
function maparea2d.metatable:on_button_press_event(event)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_button_press_event(event) then captured = true end
    end
    if captured then return true end

    local state = self:get_state()
    local gbox = self:get_selection_box()

    if event.button == 1 then
        local hovered = gbox:check_point(
            self:screenspace_to_drawspace({event.x, event.y}))
        -- Clicking inside the selection box begins a selection drag.
        if hovered == maparea2d.grabbablebox.BOX then
            local drag = MoveSelected.new(self, event.x, event.y)
            self:addListener(drag)
            drag:on_button_press_event(event)

        elseif hovered ~= maparea2d.grabbablebox.NONE then
            local scale_drag = ScaleDrag.new(
                self, event.x, event.y, hovered)
            self:addListener(scale_drag)
            scale_drag:on_button_press_event(event)

        -- Start of a BrushBox drag
        else
            local brushbox_drag = BrushBoxDrag.new(self, event.x, event.y, self.alt ~= true)
            self:addListener(brushbox_drag)
            brushbox_drag:on_button_press_event(event)
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
function maparea2d.metatable:on_button_release_event(event)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_button_release_event(event) then captured = true end
    end
    if captured then return true end

    local state = self:get_state()
    local editor = self:get_editor()

    -- Select a brush on left click.
    if event.button == 1 then
        -- Clear selection if we're only picking one at a time.
        if not state:get_multiselect() then
            editor:get_selection():clear()
        end
        -- Pick a brush based on the click position.
        local xy = self:screenspace_to_drawspace({event.x, event.y})
        local picked = self:pick_brush(xy)
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


-- Keyboard key pressed.
function maparea2d.metatable:on_key_press_event(keyval)
    -- These always need to work, and so are done before listeners.
    if keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        self.ctrl = true
    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        self.shift = true
    elseif keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.alt = true
    -- Pressing escape cancels any current actions.
    elseif keyval == LuaGDK.GDK_KEY_Escape then
        self:clearListeners()
    end

    -- Process listeners. If any return true, we're done.
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_key_press_event(keyval) then captured = true end
    end
    if captured then return true end

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
        transform:set_zoom(
            moremath.clamp(
                transform:get_zoom() * ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM
            )
        )
    elseif keyval == LuaGDK.GDK_KEY_KP_Subtract then
        transform:set_zoom(
            moremath.clamp(
                transform:get_zoom() / ZOOM_MULTIPLIER_STEP, MIN_ZOOM, MAX_ZOOM
            )
        )
    -- Reset zoom with 0.
    elseif keyval == LuaGDK.GDK_KEY_0 then
        transform:set_zoom(1.0)

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
function maparea2d.metatable:on_key_release_event(keyval)
    -- These always need to work, and so are done before listeners.
    if keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        self.ctrl = false
    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        self.shift = false
    elseif keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.alt = false
    end

    -- Process listeners. If any return true, we're done.
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_key_release_event(keyval) then captured = true end
    end
    if captured then return true end

    -- Turn off multi-brush selecting when Ctrl is released.
    if keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        local state = self:get_state()
        state:set_multiselect(false)
        self:set_state(state)

    else
        return false
    end
    return true
end


-- Mouse moved over widget.
function maparea2d.metatable:on_motion_notify_event(event)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_motion_notify_event(event) then captured = true end
    end
    if captured then return true end

    local ret = false

    local editor = self:get_editor()
    local state = self:get_state()
    local transform = self:get_transform()

    -- Change cursor if we're hovering a GrabBox selection point.
    local hovered = self:get_selection_box():check_point(
        self:screenspace_to_drawspace({event.x, event.y}))

    local CURSORS = {
        [maparea2d.grabbablebox.BOX] = "crosshair",
        [maparea2d.grabbablebox.NE] = "ne-resize",
        [maparea2d.grabbablebox.NW] = "nw-resize",
        [maparea2d.grabbablebox.SE] = "se-resize",
        [maparea2d.grabbablebox.SW] = "sw-resize",
        [maparea2d.grabbablebox.N] = "n-resize",
        [maparea2d.grabbablebox.E] = "e-resize",
        [maparea2d.grabbablebox.S] = "s-resize",
        [maparea2d.grabbablebox.W] = "w-resize"
    }
    local cursor = CURSORS[hovered] or "default"

    -- Pan view when dragging with middle-click.
    if event.state & LuaGDK.GDK_BUTTON2_MASK ~= 0 then
        transform:set_x(transform:get_x() + event.x - state:get_pointer_prev().x)
        transform:set_y(transform:get_y() + event.y - state:get_pointer_prev().y)
        state:set_pointer_prev(event)
        ret = true
    end

    self:set_cursor(cursor)
    self:set_state(state)
    self:set_transform(transform)
    return ret
end


-- Mouse scrolled on widget.
function maparea2d.metatable:on_scroll_event(event)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        if listener:on_scroll_event(event) then captured = true end
    end
    if captured then return true end

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
            transform:set_zoom(moremath.clamp(transform:get_zoom() / ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
        elseif event.direction == LuaGDK.GDK_SCROLL_UP then
            transform:set_zoom(moremath.clamp(transform:get_zoom() * ZOOM_MULTIPLIER_SMOOTH, MIN_ZOOM, MAX_ZOOM))
        end
    end

    self:set_transform(transform)
    return true
end

gAppWin.topMapArea.listeners = {}
gAppWin.rightMapArea.listeners = {}
gAppWin.frontMapArea.listeners = {}
