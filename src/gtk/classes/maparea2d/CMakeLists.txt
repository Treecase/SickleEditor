add_library(maparea2d STATIC
    BBox2View.cpp
    MapArea2D.cpp
    gbox/GrabbableBox.cpp
    gbox/GrabbableBoxView.cpp
    popup-menus/ToolPopupMenu.cpp
)
target_link_libraries(maparea2d
    PUBLIC
        editor-core
        se-lua
        PkgConfig::GTKMM
    PRIVATE
        appwin
        config
        maparea2d-components
)
target_include_directories(maparea2d PUBLIC .)

add_subdirectory(components)