pkg_check_modules(glibmm REQUIRED IMPORTED_TARGET glibmm-2.4)
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
target_compile_features(editor-world PUBLIC cxx_std_17)