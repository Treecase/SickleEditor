add_library(maparea2d-components STATIC
    BrushDraw.cpp
    DrawComponentFactory.cpp
)
target_include_directories(maparea2d-components PRIVATE .)
target_link_libraries(maparea2d-components PUBLIC editor-world PkgConfig::GTKMM)