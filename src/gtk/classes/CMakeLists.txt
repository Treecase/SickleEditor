target_sources(sickle PRIVATE
    About.cpp
    App.cpp
    gtkglutils.cpp
    LuaConsole.cpp
    MapArea3D.cpp
    WADDialog.cpp
)
target_link_libraries(sickle PRIVATE
    appwin
    config
    fgd
    luageo
    maparea2d
    rmf
    world3d
    SickleGL
)
add_subdirectory(appwin)
add_subdirectory(maparea2d)
add_subdirectory(textureselector)