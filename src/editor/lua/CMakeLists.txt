add_library(editor-lua STATIC
    BrushBox_Lua.cpp
    Editor_Lua.cpp
    EditorBrush_Lua.cpp
    Entity_Lua.cpp
    Face_Lua.cpp
    Selection_Lua.cpp
)
target_include_directories(editor-lua PRIVATE .)
target_link_libraries(editor-lua
    PUBLIC
        se-lua
    PRIVATE
        editor-core
        editor-operations
        luageo
)