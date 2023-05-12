-- BrushBoxMoveDrag
--
-- Move the BrushBox.


local BrushBoxMoveDrag = {}
BrushBoxMoveDrag.metatable = {}
BrushBoxMoveDrag.metatable.__index = BrushBoxMoveDrag.metatable


function BrushBoxMoveDrag.metatable:on_button_press_event(event)
    return true
end

function BrushBoxMoveDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function BrushBoxMoveDrag.metatable:on_key_press_event(keyval)
end

function BrushBoxMoveDrag.metatable:on_key_release_event(keyval)
end

function BrushBoxMoveDrag.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)
    local mousepos = self.maparea:screenspace_to_drawspace(event)
    if snapped then
        mousepos = geo.vector.map(round_to_grid, mousepos)
    end

    local delta = (
        self.maparea:drawspace_to_worldspace(mousepos)
        - self.maparea:drawspace_to_worldspace(self.origin))

    local brushbox = self.maparea:get_editor():get_brushbox()
    brushbox:set_start(self.bb_origin[1] + delta)
    brushbox:set_end(self.bb_origin[2] + delta)

    self.moved = true
    return true
end


function BrushBoxMoveDrag.new(parent, maparea, x, y)
    local click_pos = maparea:screenspace_to_drawspace({x, y})
    local snapped = (maparea.alt ~= true)
    if snapped then
        click_pos = geo.vector.map(round_to_grid, click_pos)
    end
    local brushbox = maparea:get_editor():get_brushbox()
    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    drag.origin = click_pos
    drag.bb_origin = {brushbox:get_start(), brushbox:get_end()}
    setmetatable(drag, BrushBoxMoveDrag.metatable)
    return drag
end

return BrushBoxMoveDrag
