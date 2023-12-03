add_library(world3d STATIC
    Brush.cpp
    DeferredExec.cpp
    Entity.cpp
    Face.cpp
    Texture.cpp
    Vertex.cpp
    World.cpp
)
target_include_directories(world3d PUBLIC include)
target_link_libraries(world3d
    PUBLIC
        PkgConfig::GTKMM
        editor-world
        glutils::glutils
        wad
)
target_compile_features(world3d PUBLIC cxx_std_17)