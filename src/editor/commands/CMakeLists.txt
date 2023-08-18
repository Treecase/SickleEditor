add_library(editor-commands STATIC
    Commands.cpp
)
target_include_directories(editor-commands PUBLIC .)
target_link_libraries(editor-commands
    PUBLIC editor-core
    PRIVATE convexhull
)