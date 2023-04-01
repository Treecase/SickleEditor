-- BrushBoxDrag
--
-- Create the brush box.


local BrushBoxDrag = {}
BrushBoxDrag.metatable = {}
BrushBoxDrag.metatable.__index = BrushBoxDrag.metatable


function BrushBoxDrag.metatable:on_button_press_event(event)
    return true
end

function BrushBoxDrag.metatable:on_button_release_event(event)
    self.maparea:removeListener(self)
    return self.moved
end

function BrushBoxDrag.metatable:on_key_press_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = false
        return true
    end
end

function BrushBoxDrag.metatable:on_key_release_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = true
        return true
    end
end

function BrushBoxDrag.metatable:on_motion_notify_event(event)
    -- Selection is cleared here since this drag can be triggered by a simple
    -- click, too.
    self.maparea:get_editor():get_selection():clear()

    local curr = self.maparea:drawspace_to_worldspace(
        self.maparea:screenspace_to_drawspace({event.x, event.y})
    )
    if self.snapped then
        curr = geo.vector.map(round_to_grid, curr)
    end
    self.maparea:get_editor():get_brushbox():set_end(curr)
    self.moved = true
    return true
end

function BrushBoxDrag.metatable:on_scroll_event(event)
end


function BrushBoxDrag.new(maparea, x, y, snapped)
    local click_pos = geo.vector.new(
        maparea:drawspace_to_worldspace(
            maparea:screenspace_to_drawspace({x, y})))

    if snapped then
        click_pos = geo.vector.map(round_to_grid, click_pos)
    end

    local brushbox = maparea:get_editor():get_brushbox()
    brushbox:set_start(click_pos)
    brushbox:set_end(click_pos)

    local drag = {}
    drag.maparea = maparea
    drag.moved = false
    drag.snapped = true
    setmetatable(drag, BrushBoxDrag.metatable)
    return drag
end

return BrushBoxDrag
