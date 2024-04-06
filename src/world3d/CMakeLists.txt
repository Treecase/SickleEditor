add_subdirectory(raycast)

add_library(world3d STATIC
    Brush.cpp
    DeferredExec.cpp
    PointEntityBox.cpp
    PointEntitySprite.cpp
    RenderComponentFactory.cpp
    SolidEntity.cpp
    Face.cpp
    Texture.cpp
    Vertex.cpp
    World.cpp
)
target_include_directories(world3d PRIVATE .)
target_link_libraries(world3d
    PUBLIC
        editor-core-gamedefinition
        editor-textures
        editor-world
        gtkglutils
        spr
        glutils::glutils
        PkgConfig::GTKMM
)
target_compile_features(world3d PUBLIC cxx_std_17)