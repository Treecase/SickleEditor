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
        PkgConfig::GTKMM
        editor-core-gamedefinition
        editor-world
        glutils::glutils
        gtkglutils
        spr
        wad
)
target_compile_features(world3d PUBLIC cxx_std_17)