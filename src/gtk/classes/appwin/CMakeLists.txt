add_library(appwin STATIC
    AppWin.cpp
    MapToolConfig.cpp
    MapTools.cpp
    ModeSelector.cpp
    OperationSearch.cpp
)
target_link_libraries(appwin
    PUBLIC
        config
        editor-core
        editor-operations
        sickle-gtk-lua
        textureselector
        PkgConfig::GTKMM
        # vvv temp vvv
        glutils::glutils # maparea3d
        world3d # maparea3d
    PRIVATE
        map
        rmf
        luageo
)
target_include_directories(appwin PUBLIC .)