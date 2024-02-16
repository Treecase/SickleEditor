add_library(editor-core STATIC
    BrushBox.cpp
    Editor.cpp
    GameDefinition.cpp
    MapTools.cpp
    Selection.cpp
)
target_include_directories(editor-core PUBLIC ..)
target_link_libraries(editor-core
    PUBLIC
        editor-world
        fgd
        se-lua
        wad
        glm::glm
        PkgConfig::sigc++
)