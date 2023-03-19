-- ScaleDrag
--
-- Scale the selected brushes.

local ScaleDrag = {}
ScaleDrag.metatable = {}
ScaleDrag.metatable.__index = ScaleDrag.metatable


function ScaleDrag.metatable:on_button_press_event(event)
end

function ScaleDrag.metatable:on_button_release_event(event)
    self.maparea:removeListener(self)
    return true
end

function ScaleDrag.metatable:on_key_press_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = false
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        self.centered = true
    else
        return false
    end
    return true
end

function ScaleDrag.metatable:on_key_release_event(keyval)
    if keyval == LuaGDK.GDK_KEY_Alt_L or keyval == LuaGDK.GDK_KEY_Alt_R then
        self.snapped = true
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        self.centered = false
    else
        return false
    end
    return true
end

function ScaleDrag.metatable:on_motion_notify_event(event)
    -- Point to scale from.
    local center = geo.vector.new()

    -- Scale out from center.
    if self.centered then
        local count = 0.0
        for brush in self.maparea:get_editor():get_selection():iterate() do
            for i,vertex in ipairs(brush:get_vertices()) do
                center = center + vertex
                count = count + 1
            end
        end
        center = center / count

    -- Scale from opposite corner.
    else
        local min_x,max_x, min_y,max_y = nil,nil, nil,nil

        for brush in self.maparea:get_editor():get_selection():iterate() do
            for _,v in ipairs(brush:get_vertices()) do
                local xy = self.maparea:worldspace_to_drawspace(v)
                if not min_x or xy.x < min_x then min_x = xy.x end
                if not max_x or xy.x > max_x then max_x = xy.x end
                if not min_y or xy.y < min_y then min_y = xy.y end
                if not max_y or xy.y > max_y then max_y = xy.y end
            end
        end

        local minmax = {
            ["N"] = min_y,
            ["E"] = max_x,
            ["S"] = max_y,
            ["W"] = min_x,
            ["x"] = min_x + (max_x - min_x) * 0.5,
            ["y"] = min_y + (max_y - min_y) * 0.5
        }

        -- indexes into minmax
        local COORD_MAPPING = {
            [grabbablebox.NE] = {"S", "W"},
            [grabbablebox.NW] = {"S", "E"},
            [grabbablebox.SE] = {"N", "W"},
            [grabbablebox.SW] = {"N", "E"},
            [grabbablebox.N ] = {"S", "x"},
            [grabbablebox.E ] = {"y", "W"},
            [grabbablebox.S ] = {"N", "x"},
            [grabbablebox.W ] = {"y", "E"}
        }

        local coords = COORD_MAPPING[self.handle]
        center = self.maparea:drawspace_to_worldspace(
            {minmax[coords[2]], minmax[coords[1]]})
    end

    local SCALE_FACTOR_FACTOR = {
        [grabbablebox.NE] = {1, 1},
        [grabbablebox.NW] = {1, 1},
        [grabbablebox.SE] = {1, 1},
        [grabbablebox.SW] = {1, 1},
        [grabbablebox.N ] = {0, 1},
        [grabbablebox.E ] = {1, 0},
        [grabbablebox.S ] = {0, 1},
        [grabbablebox.W ] = {1, 0}
    }
    local scale_factor_factor = geo.vector.map(
        math.abs,
        self.maparea:drawspace_to_worldspace({
            SCALE_FACTOR_FACTOR[self.handle][1],
            SCALE_FACTOR_FACTOR[self.handle][2]}))

    local curr = self.maparea:drawspace_to_worldspace(
        self.maparea:screenspace_to_drawspace({event.x, event.y}))
    local prev = self.maparea:drawspace_to_worldspace(
        self.maparea:screenspace_to_drawspace({self.x, self.y}))

    if self.snapped then
        local grid = gAppWin:get_grid_size()
        curr = geo.vector.map(math.floor, curr / grid) * grid
        prev = geo.vector.map(math.floor, prev / grid) * grid
    end

    -- Calculate distance from selection centerpoint
    local radius_curr = geo.vector.map(math.abs, curr - center)
    local radius_prev = geo.vector.map(
        function(x) if x ~= 0 then return math.abs(x) else return 1 end end,
        prev - center)

    -- Calculate scaling vector.
    local scale_vector = geo.vector.map(
        function(x) if x ~= 0 then return x else return 1 end end,
        (radius_curr / radius_prev) * scale_factor_factor)

    -- Create scale matrix
    local scalemat = geo.matrix.scale(geo.matrix.new(), scale_vector)

    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:transform(
            geo.matrix.translate(geo.matrix.new(), center)
            * scalemat
            * geo.matrix.translate(geo.matrix.new(), -center))
    end

    self.x = event.x
    self.y = event.y
    self.moved = true

    return true
end

function ScaleDrag.metatable:on_scroll_event(event)
end


function ScaleDrag.new(maparea, x, y, origin, handle)
    local scale_drag = {}
    scale_drag.maparea = maparea
    scale_drag.x = x
    scale_drag.y = y
    scale_drag.moved = false
    scale_drag.snapped = true
    scale_drag.centered = false
    scale_drag.origin = origin
    scale_drag.handle = handle

    setmetatable(scale_drag, ScaleDrag.metatable)

    return scale_drag
end

return ScaleDrag
