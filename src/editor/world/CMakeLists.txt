add_library(editor-world STATIC
    Brush.cpp
    Entity.cpp
    Face.cpp
    World.cpp
)
target_include_directories(editor-world PRIVATE .)
target_link_libraries(editor-world
    PUBLIC
        config
        convexhull
        editor-core
        editor-interfaces
        map
        rmf
        se-lua
        glm::glm
        PkgConfig::glibmm
)