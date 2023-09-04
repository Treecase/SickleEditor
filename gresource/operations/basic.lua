add_operation(
    "Basic", "Translate", "brush", "fff",
    function(brush, x, y, z)
        print("Translate brush: ", x, y, z)
        for _,b in ipairs(brush) do print(b) end
    end)
add_operation(
    "Basic", "Rotate", "brush", "fff",
    function(brush, x, y, z)
        print("Rotate brush: ", x, y, z)
        for _,b in ipairs(brush) do print(b) end
    end)
add_operation(
    "Basic", "Scale", "brush", "fff",
    function(brush, x, y, z)
        print("Scale brush: ", x, y, z)
        for _,b in ipairs(brush) do print(b) end
    end)
-- temp
add_operation(
    "Basic", "Transform", "brush", "",
    function(brush)
        print("Transform")
        for _,b in ipairs(brush) do print(b) end
    end)

add_operation(
    "Brush",
    "CreateFromBox",
    "editor",
    "",
    function(editor)
        local bb = editor:get_brushbox()
        local a = bb:get_start()
        local b = bb:get_end()
        bb:set_start(geo.vector.new())
        bb:set_end(geo.vector.new())

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
    end
)