-- BrushBoxHandleDrag
--
-- BrushBox handle dragged.


local BrushBoxHandleDrag = {}
BrushBoxHandleDrag.metatable = {}
BrushBoxHandleDrag.metatable.__index = BrushBoxHandleDrag.metatable


local function corners(box, maparea)
    local points = {box:get_start(), box:get_end()}

    local northwest = maparea:worldspace_to_drawspace(points[1])
    local southeast = maparea:worldspace_to_drawspace(points[1])

    for _,point in ipairs(points) do
        local point = maparea:worldspace_to_drawspace(point)
        if point.x < northwest.x then
            northwest.x = point.x
        end
        if point.y < northwest.y then
            northwest.y = point.y
        end
        if point.x > southeast.x then
            southeast.x = point.x
        end
        if point.y > southeast.y then
            southeast.y = point.y
        end
    end

    return {northwest, southeast}
end


function BrushBoxHandleDrag.metatable:on_button_press_event(event)
    return true
end

function BrushBoxHandleDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function BrushBoxHandleDrag.metatable:on_key_press_event(keyval)
end

function BrushBoxHandleDrag.metatable:on_key_release_event(keyval)
end

function BrushBoxHandleDrag.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)
    local mousepos = self.maparea:screenspace_to_drawspace(event)
    if snapped then
        mousepos = geo.vector.map(round_to_grid, mousepos)
    end

    if (    self.handle == maparea2d.grabbablebox.N
            or self.handle == maparea2d.grabbablebox.S) then
        mousepos.x = self.orig_cardinals.E.x
        self.anchor.x = self.orig_cardinals.W.x
    elseif (self.handle == maparea2d.grabbablebox.E
            or self.handle == maparea2d.grabbablebox.W) then
        mousepos.y = self.orig_cardinals.N.y
        self.anchor.y = self.orig_cardinals.S.y
    end

    local brushbox = self.maparea:get_editor():get_brushbox()
    brushbox:set_start(self.maparea:drawspace_to_worldspace(self.anchor))
    brushbox:set_end(self.maparea:drawspace_to_worldspace(mousepos))
end


function BrushBoxHandleDrag.new(parent, maparea, x, y, handle)
    local corners_ = corners(maparea:get_editor():get_brushbox(), maparea)
    local CARDINAL = {
        ["N"] = geo.vector.new(0, corners_[1].y),
        ["E"] = geo.vector.new(corners_[2].x, 0),
        ["S"] = geo.vector.new(0, corners_[2].y),
        ["W"] = geo.vector.new(corners_[1].x, 0),
    }
    local OPPOSITE_CORNER = {
        [maparea2d.grabbablebox.NE] = CARDINAL.S + CARDINAL.W,
        [maparea2d.grabbablebox.NW] = CARDINAL.S + CARDINAL.E,
        [maparea2d.grabbablebox.SE] = CARDINAL.N + CARDINAL.W,
        [maparea2d.grabbablebox.SW] = CARDINAL.N + CARDINAL.E,
        [maparea2d.grabbablebox.N] = CARDINAL.S,
        [maparea2d.grabbablebox.E] = CARDINAL.W,
        [maparea2d.grabbablebox.S] = CARDINAL.N,
        [maparea2d.grabbablebox.W] = CARDINAL.E,
    }

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    drag.handle = handle
    drag.anchor = OPPOSITE_CORNER[handle]
    drag.orig_cardinals = CARDINAL
    setmetatable(drag, BrushBoxHandleDrag.metatable)
    return drag
end

return BrushBoxHandleDrag
