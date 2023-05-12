-- BrushBox
--
-- Manage BrushBox interactions.

local BrushBoxCreate = require "MapArea2D/BrushBox/BrushBoxCreate"
local BrushBoxHandleDrag = require "MapArea2D/BrushBox/BrushBoxHandleDrag"
local BrushBoxMoveDrag = require "MapArea2D/BrushBox/BrushBoxMoveDrag"
local EventListener = require "EventListener"


local BrushBox = {}
BrushBox.metatable = {}
BrushBox.metatable.__index = BrushBox.metatable

EventListener.inherit(BrushBox.metatable)


function BrushBox.metatable:on_button_press_event(event)
    if self:doEvent("on_button_press_event", event) then return true end

    local hovered = self.maparea:get_brushbox():check_point(
        self.maparea:screenspace_to_drawspace(event))

    if event.button == 1 then
        if hovered == maparea2d.grabbablebox.NONE then
            self:addListener(
                BrushBoxCreate.new(self, self.maparea, event.x, event.y))

        elseif hovered == maparea2d.grabbablebox.BOX then
            self:addListener(
                BrushBoxMoveDrag.new(self, self.maparea, event.x, event.y))

        else
            self:addListener(
                BrushBoxHandleDrag.new(
                    self, self.maparea, event.x, event.y, hovered))

        end
    end
end

function BrushBox.metatable:on_button_release_event(event)
    if self:doEvent("on_button_release_event", event) then return true end
end

function BrushBox.metatable:on_key_press_event(keyval)
    if self:doEvent("on_key_press_event", event) then return true end
end

function BrushBox.metatable:on_key_release_event(keyval)
    if self:doEvent("on_key_release_event", event) then return true end
end

function BrushBox.metatable:on_motion_notify_event(event)
    if self:doEvent("on_motion_notify_event", event) then return true end

    local mouse_position_ds = self.maparea:screenspace_to_drawspace(event)
    local hovered = self.maparea:get_brushbox():check_point(mouse_position_ds)
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
    local cursor = CURSORS[hovered]
    if cursor then
        self.maparea:set_cursor(CURSORS[hovered])
        return true
    end
end

function BrushBox.metatable:on_scroll_event(event)
    if self:doEvent("on_scroll_event", event) then return true end
end


function BrushBox.new(maparea)
    local brushbox = {}
    brushbox.maparea = maparea
    setmetatable(brushbox, BrushBox.metatable)
    EventListener.construct(brushbox)
    return brushbox
end

return BrushBox
