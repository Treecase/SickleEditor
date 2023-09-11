-- TODO
add_operation(
    "Basic", "Rotate", "brush", {"f", "f", "f"},--{"vec3"},
    function(editor, brushes, x, y, z)
        print("Rotate brush: ", x, y, z)
        for _,b in ipairs(brushes) do print(b) end
    end)

-- add_operation(
--     "Basic", "Transform", "brush", {"mat4"},
--     function(editor, brushes)
--         print("Transform")
--         for _,b in ipairs(brushes) do print(b) end
--     end)


add_operation(
    "Brush", "Translate", "brush", {"vec3"},
    function(editor, brushes, delta)
        for _,b in ipairs(brushes) do
            b:translate(delta)
        end
    end)

add_operation(
    "Brush", "Scale", "brush", {"vec3"},
    function(editor, brushes, scale)
        for _,b in ipairs(brushes) do
            b:scale(scale)
        end
    end,
    {geo.vec3.new(1, 1, 1)})

add_operation(
    "Brush",
    "CreateFromBox",
    "brush",
    {},
    function(editor, _)
        local bb = editor:get_brushbox()
        local a = bb:get_start()
        local b = bb:get_end()
        bb:set_start(geo.vec3.new())
        bb:set_end(geo.vec3.new())
        editor:add_brush(
            {a.x, a.y, a.z},
            {a.x, a.y, b.z},
            {a.x, b.y, a.z},
            {a.x, b.y, b.z},
            {b.x, a.y, a.z},
            {b.x, a.y, b.z},
            {b.x, b.y, a.z},
            {b.x, b.y, b.z}
        )
    end)

add_operation(
    "Brush",
    "DeleteSelected",
    "brush",
    {},
    function(editor, _)
        local sel = editor:get_selection()
        for brush in sel:iterate() do
            editor:remove_brush(brush)
        end
    end)
