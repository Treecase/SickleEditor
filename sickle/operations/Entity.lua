add_operation(
    "Entity",
    "Create",
    "entity",
    {"string"},
    function(editor, _, classname)
        editor:add_entity(classname)
    end)