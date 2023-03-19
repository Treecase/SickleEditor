-- SelectionDrag
--
-- Move the selected brushes.

local MoveSelected = {}
MoveSelected.metatable = {}
MoveSelected.metatable.__index = MoveSelected.metatable


function MoveSelected.metatable:on_button_press_event(event)
end

function MoveSelected.metatable:on_button_release_event(event)
    self.maparea:removeListener(self)
    return self.moved
end

function MoveSelected.metatable:on_key_press_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = false
        return true
    end
end

function MoveSelected.metatable:on_key_release_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = true
        return true
    end
end

function MoveSelected.metatable:on_motion_notify_event(event)
    local curr = self.maparea:drawspace_to_worldspace(
        self.maparea:screenspace_to_drawspace({event.x, event.y}))
    local prev = self.maparea:drawspace_to_worldspace(
        self.maparea:screenspace_to_drawspace({self.x, self.y}))

    local delta = curr - prev
    local accum = self.accum + delta

    local rounded = accum
    local rounded_prev = self.accum
    if self.snapped then
        local grid = gAppWin:get_grid_size()
        rounded = geo.vector.map(math.floor, rounded / grid) * grid
        rounded_prev = geo.vector.map(math.floor, rounded_prev / grid) * grid
    end

    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:translate(rounded - rounded_prev)
    end

    self.x = event.x
    self.y = event.y
    self.moved = true
    self.accum = accum
    return true
end

function MoveSelected.metatable:on_scroll_event(event)
end


function MoveSelected.new(maparea, x, y)
    local drag = {}
    drag.maparea = maparea
    drag.x = x
    drag.y = y
    drag.moved = false
    drag.snapped = true
    drag.accum = geo.vector.new()

    setmetatable(drag, MoveSelected.metatable)

    return drag
end

return MoveSelected
