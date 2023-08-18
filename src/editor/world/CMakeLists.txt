add_library(editor-world STATIC
    Brush.cpp
    Entity.cpp
    Face.cpp
    Map.cpp
)
target_include_directories(editor-world PUBLIC ..)
target_link_libraries(editor-world
    PUBLIC
        convexhull
        map
        rmf
        glm::glm
        PkgConfig::sigc++
)