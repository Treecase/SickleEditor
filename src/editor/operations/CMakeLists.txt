add_library(editor-operations STATIC
    Operation.cpp
)
target_include_directories(editor-operations PUBLIC ..)
target_link_libraries(editor-operations
    PUBLIC
        editor-core
        se-lua
)