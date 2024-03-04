add_subdirectory(gamedefinition)

add_library(editor-core STATIC
    BrushBox.cpp
    Editor.cpp
    MapTools.cpp
    Selection.cpp
)
target_include_directories(editor-core PRIVATE .)
target_link_libraries(editor-core
    PUBLIC
        editor-world
        se-lua
        wad
        glm::glm
        PkgConfig::sigc++
)