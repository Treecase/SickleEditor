add_library(editor-core STATIC
    BrushBox.cpp
    Editor.cpp
    MapTools.cpp
    Selection.cpp
)
target_include_directories(editor-core PUBLIC ..)
target_link_libraries(editor-core
    PUBLIC
        editor-world
        se-lua
        glm::glm
        PkgConfig::sigc++
    PRIVATE
        editor-commands
)