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