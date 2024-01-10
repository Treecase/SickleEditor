add_operation(
    "Object", "Delete",
    "object",
    {},
    function(editor, objects)
        for _,obj in ipairs(objects) do
            editor:remove_object(obj)
        end
    end
)