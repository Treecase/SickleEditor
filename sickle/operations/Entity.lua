add_operation(
    "Entity",
    "Create",
    "entity",
    {"string"},
    function(editor, _, classname)
        pcall(editor.add_entity, editor, classname)
    end)