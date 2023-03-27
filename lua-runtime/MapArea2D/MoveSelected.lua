-- SelectionDrag
--
-- Move the selected brushes.
-- Snaps to the bounding-box corner closest to where the user clicked to start
-- the drag.

local function find_corners(brushes)
    local topleft = {nil, nil}
    local bottomright = {nil, nil}
    for brush in brushes do
        for i,vertex in ipairs(brush:get_vertices()) do
            if topleft[1] == nil or vertex.x < topleft[1] then
                topleft[1] = vertex.x
            end
            if bottomright[1] == nil or vertex.x > bottomright[1] then
                bottomright[1] = vertex.x
            end
            if topleft[2] == nil or vertex.y > topleft[2] then
                topleft[2] = vertex.y
            end
            if bottomright[2] == nil or vertex.y < bottomright[2] then
                bottomright[2] = vertex.y
            end
        end
    end
    return geo.vector.new(topleft), geo.vector.new(bottomright)
end


local MoveSelected = {}
MoveSelected.metatable = {}
MoveSelected.metatable.__index = MoveSelected.metatable


function MoveSelected.metatable:on_button_press_event(event)
    return true
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

    local newcorner = curr
    if self.snapped then
        newcorner = geo.vector.map(round_to_grid, curr)
    end

    local translation = newcorner - self.corner
    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:translate(translation - self.prev_translation)
    end
    self.prev_translation = translation

    self.moved = true
    return true
end

function MoveSelected.metatable:on_scroll_event(event)
end


function MoveSelected.new(maparea, x, y)
    local click_pos = maparea:drawspace_to_worldspace(
        maparea:screenspace_to_drawspace({x, y}))

    local topleft, bottomright = find_corners(
        maparea:get_editor():get_selection():iterate())
    local corners = {
        topleft,
        geo.vector.new(bottomright.x, topleft.y),
        geo.vector.new(topleft.x, bottomright.y),
        bottomright
    }

    local closest_corner = nil
    for i,corner in ipairs(corners) do
        local distance = geo.vector.length(corner - click_pos)
        if closest_corner == nil or distance < closest_corner[2] then
            closest_corner = {corner, distance}
        end
    end

    local drag = {}
    drag.maparea = maparea
    drag.moved = false
    drag.snapped = true
    drag.corner = closest_corner[1]
    drag.prev_translation = geo.vector.new()
    setmetatable(drag, MoveSelected.metatable)
    return drag
end

return MoveSelected
