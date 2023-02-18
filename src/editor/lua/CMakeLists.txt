
add_library(editor-lua STATIC Editor_Lua.cpp Selection_Lua.cpp)
target_include_directories(editor-lua PUBLIC .)
target_link_libraries(editor-lua PRIVATE editor)
target_link_libraries(editor-lua PUBLIC se-lua)
