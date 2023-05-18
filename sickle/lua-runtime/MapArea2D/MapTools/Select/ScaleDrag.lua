-- ScaleDrag
--
-- Scale the selected brushes.
-- Hold Ctrl to scale from the center of the brush.

local ScaleDrag = {}
ScaleDrag.metatable = {}
ScaleDrag.metatable.__index = ScaleDrag.metatable


-- Get the centerpoint of selected brushes.
local function get_center(drag)
    local center = geo.vector.new()

    -- Scale out from center.
    if drag:centered() then
        local count = 0.0
        for brush in drag.maparea:get_editor():get_selection():iterate() do
            for i,vertex in ipairs(brush:get_vertices()) do
                center = center + vertex
                count = count + 1
            end
        end
        center = drag.maparea:worldspace_to_drawspace(center / count)

    -- Scale from opposite corner.
    else
        local min_x,max_x, min_y,max_y = nil,nil, nil,nil

        for brush in drag.maparea:get_editor():get_selection():iterate() do
            for _,v in ipairs(brush:get_vertices()) do
                local xy = drag.maparea:worldspace_to_drawspace(v)
                if min_x == nil or xy.x < min_x then min_x = xy.x end
                if max_x == nil or xy.x > max_x then max_x = xy.x end
                if min_y == nil or xy.y < min_y then min_y = xy.y end
                if max_y == nil or xy.y > max_y then max_y = xy.y end
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
            [maparea2d.grabbablebox.NE] = {"S", "W"},
            [maparea2d.grabbablebox.NW] = {"S", "E"},
            [maparea2d.grabbablebox.SE] = {"N", "W"},
            [maparea2d.grabbablebox.SW] = {"N", "E"},
            [maparea2d.grabbablebox.N ] = {"S", "x"},
            [maparea2d.grabbablebox.E ] = {"y", "W"},
            [maparea2d.grabbablebox.S ] = {"N", "x"},
            [maparea2d.grabbablebox.W ] = {"y", "E"}
        }

        local coords = COORD_MAPPING[drag.handle]
        center = geo.vector.new(minmax[coords[2]], minmax[coords[1]])
    end

    return center
end

-- Divide two vectors, skipping if the denominator element is 0.
local function safe_vector_divide(a, b)
    local function make_safe(x)
        if x == 0.0 then
            return 1.0
        else
            return x
        end
    end
    return a / geo.vector.map(make_safe, b)
end


function ScaleDrag.metatable:snapped()
    return self.maparea.alt == false
end

function ScaleDrag.metatable:centered()
    return self.maparea.ctrl == true
end


function ScaleDrag.metatable:on_button_press_event(event)
    return true
end

function ScaleDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return true
end

function ScaleDrag.metatable:on_key_press_event(event)
end

function ScaleDrag.metatable:on_key_release_event(event)
end

function ScaleDrag.metatable:on_motion_notify_event(event)
    -- Point to scale from.
    local center = get_center(self)

    local SCALE_DIRECTIONS = {
        [maparea2d.grabbablebox.NE] = {1, 1},
        [maparea2d.grabbablebox.NW] = {1, 1},
        [maparea2d.grabbablebox.SE] = {1, 1},
        [maparea2d.grabbablebox.SW] = {1, 1},
        [maparea2d.grabbablebox.N ] = {0, 1},
        [maparea2d.grabbablebox.E ] = {1, 0},
        [maparea2d.grabbablebox.S ] = {0, 1},
        [maparea2d.grabbablebox.W ] = {1, 0}
    }
    local scale_directions = geo.vector.map(
        math.abs,
        self.maparea:drawspace_to_worldspace({
            SCALE_DIRECTIONS[self.handle][1],
            SCALE_DIRECTIONS[self.handle][2]}
        )
    )

    local click_pos = self.maparea:screenspace_to_drawspace({event.x, event.y})
    if self:snapped() then
        click_pos = geo.vector.map(round_to_grid, click_pos)
    end

    local distance = geo.vector.map(math.abs, click_pos - center)
    local scale = safe_vector_divide(distance, self.base_distance)

    if self:centered() then
        scale = scale * 2
    end

    -- Prevent scaling to 0, otherwise we won't be able to scale back up!
    for _,i in ipairs({"x", "y", "z", "w"}) do
        if scale_directions[i] == 0 then
            scale[i] = 1
        elseif scale[i] == 0 then
            scale[i] = self.prev_scale[i]
        end
    end

    local scale_mat = geo.matrix.scale(geo.matrix.new(), scale)
    local inverse_prev_scale_mat = geo.matrix.scale(
        geo.matrix.new(),
        safe_vector_divide(1.0, self.prev_scale)
    )

    local center_ws = self.maparea:drawspace_to_worldspace(center)
    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:transform(
            geo.matrix.translate(geo.matrix.new(), center_ws)
            * scale_mat
            * inverse_prev_scale_mat
            * geo.matrix.translate(geo.matrix.new(), -center_ws)
        )
    end

    self.prev_scale = scale
    self.moved = true
    return true
end

function ScaleDrag.metatable:on_scroll_event(event)
end


function ScaleDrag.new(parent, maparea, x, y, handle)
    local scale_drag = {}
    scale_drag.parent = parent
    scale_drag.maparea = maparea
    scale_drag.handle = handle
    scale_drag.moved = false

    setmetatable(scale_drag, ScaleDrag.metatable)

    local click_pos = maparea:screenspace_to_drawspace({x, y})
    if scale_drag:snapped() then
        click_pos = geo.vector.map(round_to_grid, click_pos)
    end

    local center = get_center(scale_drag)
    local distance = geo.vector.map(math.abs, click_pos - center)
    scale_drag.base_distance = distance
    scale_drag.prev_scale = safe_vector_divide(distance, distance)

    return scale_drag
end

return ScaleDrag
