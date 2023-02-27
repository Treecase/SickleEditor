
add_library(editor-lua STATIC
    BrushBox_Lua.cpp
    Editor_Lua.cpp
    EditorBrush_Lua.cpp
    Selection_Lua.cpp
)
target_include_directories(editor-lua PUBLIC .)
target_link_libraries(editor-lua PRIVATE editor luageo PUBLIC se-lua)
