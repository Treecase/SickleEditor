
local moremath = require "moremath"
local EventListener = require "EventListener"
local MapTools = require "MapArea2D/MapTools/MapTools"
local ViewDrag = require "MapArea2D/ViewDrag"


-- Constants for panning/zoom control.
local MOVE_SMOOTH = 8.0
local MOVE_STEP = 64.0
local ZOOM_MULTIPLIER_SMOOTH = 1.1
local ZOOM_MULTIPLIER_STEP = 2.0
local MIN_ZOOM = 1.0 / 16.0
local MAX_ZOOM = 16.0


EventListener.inherit(maparea2d.metatable)


-- Mouse button pressed.
function maparea2d.metatable:on_button_press_event(event)
    if self:doEvent("on_button_press_event", event) then return true end

    -- Middle click begins view panning.
    if event.button == 2 then
        local view_drag = ViewDrag.new(self, event.x, event.y)
        self:addListener(view_drag)
        return true
    end
end


-- Mouse button released.
function maparea2d.metatable:on_button_release_event(event)
    if self:doEvent("on_button_release_event", event) then return true end
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
    end

    if self:doEvent("on_key_press_event", event) then return true end

    local transform = self:get_transform()

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

    else
        return false
    end

    self:set_transform(transform)
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

    if self:doEvent("on_key_release_event", event) then return true end
end


-- Mouse moved over widget.
function maparea2d.metatable:on_motion_notify_event(event)
    if self:doEvent("on_motion_notify_event", event) then return true end

    -- Change cursor if we're hovering a GrabBox selection point.
    local grabbableboxes = {
        self:get_selection_box(),
    }
    local hovered = maparea2d.grabbablebox.NONE
    local mouse_position_ds = self:screenspace_to_drawspace(event)
    for _,gbox in ipairs(grabbableboxes) do
        local handle = gbox:check_point(mouse_position_ds)
        if hovered == maparea2d.grabbablebox.NONE then
            hovered = handle
        end
    end
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
    self:set_cursor(CURSORS[hovered] or "default")

    return true
end


-- Mouse scrolled on widget.
function maparea2d.metatable:on_scroll_event(event)
    if self:doEvent("on_scroll_event", event) then return true end

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


function maparea2d.metatable:on_maptool_changed()
    local Tool = MapTools[gAppWin:get_maptool()]
    self:removeListener(self.tool)
    self.tool = Tool.new(self)
    self:addListener(self.tool)
end


local function setup(maparea)
    maparea.shift = false
    maparea.ctrl = false
    maparea.alt = false
    maparea.tool = MapTools[gAppWin:get_maptool()].new(maparea)
    EventListener.construct(maparea)
    maparea:addListener(maparea.tool)
    gAppWin:addListener(maparea)
end

setup(gAppWin.topMapArea)
setup(gAppWin.rightMapArea)
setup(gAppWin.frontMapArea)
