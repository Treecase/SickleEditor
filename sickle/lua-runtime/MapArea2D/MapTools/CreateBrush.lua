-- CreateBrush
--
-- Manage BrushBox interactions.

local EventListener = require "EventListener"
local Create = require "MapArea2D/MapTools/CreateBrush/Create"
local HandleDrag = require "MapArea2D/MapTools/CreateBrush/HandleDrag"
local MoveDrag = require "MapArea2D/MapTools/CreateBrush/MoveDrag"


local CreateBrush = {}
CreateBrush.metatable = {}
CreateBrush.metatable.__index = CreateBrush.metatable

EventListener.inherit(CreateBrush.metatable)


local function brush_from_brushbox(brushbox)
    local a = brushbox:get_start()
    local b = brushbox:get_end()
    local vertices = {
        {a.x, a.y, a.z},
        {a.x, a.y, b.z},
        {a.x, b.y, a.z},
        {a.x, b.y, b.z},
        {b.x, a.y, a.z},
        {b.x, a.y, b.z},
        {b.x, b.y, a.z},
        {b.x, b.y, b.z},
    }
    return vertices
end


function CreateBrush.metatable:on_removed()
    local brushbox = self.maparea:get_editor():get_brushbox()
    brushbox:set_start(geo.vector.new())
    brushbox:set_end(geo.vector.new())
end


function CreateBrush.metatable:on_button_press_event(event)
    if self:doEvent("on_button_press_event", event) then return true end

    local hovered = self.maparea:get_brushbox():check_point(
        self.maparea:screenspace_to_drawspace(event))

    if event.button == 1 then
        if hovered == maparea2d.grabbablebox.NONE then
            self:addListener(Create.new(self, self.maparea, event.x, event.y))

        elseif hovered == maparea2d.grabbablebox.BOX then
            self:addListener(MoveDrag.new(self, self.maparea, event.x, event.y))

        else
            self:addListener(
                HandleDrag.new(self, self.maparea, event.x, event.y, hovered))

        end
    end
end

function CreateBrush.metatable:on_button_release_event(event)
    if self:doEvent("on_button_release_event", event) then return true end
end

function CreateBrush.metatable:on_key_press_event(event)
    if self:doEvent("on_key_press_event", event) then return true end

    if event == LuaGDK.GDK_KEY_Return then
        local brushbox = self.maparea:get_editor():get_brushbox()
        self.maparea:get_editor():add_brush(brush_from_brushbox(brushbox))
        brushbox:set_start(geo.vector.new())
        brushbox:set_end(geo.vector.new())
        return true
    end
end

function CreateBrush.metatable:on_key_release_event(event)
    if self:doEvent("on_key_release_event", event) then return true end
end

function CreateBrush.metatable:on_motion_notify_event(event)
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

function CreateBrush.metatable:on_scroll_event(event)
    if self:doEvent("on_scroll_event", event) then return true end
end


function CreateBrush.new(maparea)
    local brushbox = {}
    brushbox.maparea = maparea
    setmetatable(brushbox, CreateBrush.metatable)
    EventListener.construct(brushbox)
    return brushbox
end

return CreateBrush
