add_library(maparea2d-components STATIC
    BBoxComponentFactory.cpp
    BrushBBox.cpp
    BrushDraw.cpp
    DrawComponentFactory.cpp
    EntityBBox.cpp
    EntityDraw.cpp
)
target_include_directories(maparea2d-components PRIVATE .)
target_link_libraries(maparea2d-components PUBLIC
    editor-world
    utils
    PkgConfig::GTKMM
)