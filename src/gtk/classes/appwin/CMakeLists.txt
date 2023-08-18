add_library(appwin STATIC
    AppWin.cpp
    OperationSearch.cpp
)
target_link_libraries(appwin
    PUBLIC
        editor-core
        editor-operations
        sickle-gtk-lua
        PkgConfig::GTKMM
        # vvv temp vvv
        glutils::glutils # maparea3d
        world3d # maparea3d
    PRIVATE
        config
        map
        rmf
        luageo
)
target_include_directories(appwin PUBLIC .)