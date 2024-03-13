add_library(maparea3d STATIC MapArea3D.cpp)
target_link_libraries(maparea3d
    PUBLIC
        editor-core
        se-lua
        utils
        gtkglutils
        world3d
        PkgConfig::GTKMM
    PRIVATE
        world3d-raycast
)

target_sources(sickle PRIVATE
    About.cpp
    App.cpp
)
target_link_libraries(sickle PRIVATE
    appwin
    config
    fgd
    luageo
    maparea2d
    maparea3d
    preferences
    rmf
    SickleGL
)
add_subdirectory(appwin)
add_subdirectory(luaconsole)
add_subdirectory(luadebugger)
add_subdirectory(maparea2d)
add_subdirectory(preferences)
add_subdirectory(textureselector)