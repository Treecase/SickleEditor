-- BrushBoxCreate
--
-- Create the brush box.


local BrushBoxCreate = {}
BrushBoxCreate.metatable = {}
BrushBoxCreate.metatable.__index = BrushBoxCreate.metatable


function BrushBoxCreate.metatable:on_button_press_event(event)
    return true
end

function BrushBoxCreate.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function BrushBoxCreate.metatable:on_key_press_event(keyval)
end

function BrushBoxCreate.metatable:on_key_release_event(keyval)
end

function BrushBoxCreate.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)

    -- Selection is cleared here since this drag can be triggered by a simple
    -- click, too.
    self.maparea:get_editor():get_selection():clear()

    local curr = self.maparea:screenspace_to_drawspace(event)
    if snapped then
        curr = geo.vector.map(round_to_grid, curr)
    end
    self.maparea:get_editor():get_brushbox():set_end(
        self.maparea:drawspace_to_worldspace(curr))
    self.moved = true
    return true
end

function BrushBoxCreate.metatable:on_scroll_event(event)
end


function BrushBoxCreate.new(parent, maparea, x, y)
    local click_pos = maparea:screenspace_to_drawspace({x, y})
    local snapped = (maparea.alt ~= true)
    if snapped then
        click_pos = geo.vector.map(round_to_grid, click_pos)
    end

    local brushbox = maparea:get_editor():get_brushbox()
    brushbox:set_start(maparea:drawspace_to_worldspace(click_pos))
    brushbox:set_end(maparea:drawspace_to_worldspace(click_pos))

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    setmetatable(drag, BrushBoxCreate.metatable)
    return drag
end

return BrushBoxCreate
