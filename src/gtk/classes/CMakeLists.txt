
target_sources(sickle PRIVATE
    About.cpp
    App.cpp
    AppWin.cpp
    gtkglutils.cpp
    LuaConsole.cpp
    MapArea2D.cpp
    MapArea3D.cpp
    MapTools.cpp
    WADDialog.cpp
)

target_link_libraries(sickle PRIVATE config fgd luageo rmf world3d)
