add_operation(
    "Basic", "Translate", "brush", "fff",
    function(brush, x, y, z)
        print("Translate brush: ", x, y, z)
    end)
add_operation(
    "Basic", "Rotate", "brush", "fff",
    function(brush, x, y, z)
        print("Rotate brush: ", x, y, z)
    end)
add_operation(
    "Basic", "Scale", "brush", "fff",
    function(brush, x, y, z)
        print("Scale brush: ", x, y, z)
    end)
-- temp
add_operation(
    "Basic", "Transform", "brush", "",
    function(brush)
        print("Transform")
    end)