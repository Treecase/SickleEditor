
target_sources(sickle PRIVATE
    App.cpp
    About.cpp
    AppWin.cpp
    LuaConsole.cpp
    MapArea.cpp
    MapArea2D.cpp
    gtkglutils.cpp
)

target_link_libraries(sickle PRIVATE luageo)
