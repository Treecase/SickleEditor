
target_sources(sickle PRIVATE
    About.cpp
    App.cpp
    AppWin.cpp
    gtkglutils.cpp
    LuaConsole.cpp
    MapArea2D.cpp
    MapArea3D.cpp
    WADDialog.cpp
)

target_link_libraries(sickle PRIVATE luageo rmf world3d)
