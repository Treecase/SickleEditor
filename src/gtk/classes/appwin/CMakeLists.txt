add_subdirectory(propertyeditor)

add_library(appwin STATIC
    AppWin.cpp
    FaceEditor.cpp
    MapTools.cpp
    ModeSelector.cpp
    OperationParameterEditor.cpp
    OperationSearch.cpp
    Outliner.cpp
)
target_link_libraries(appwin
    PUBLIC
        appwin-propertyeditor
        config
        editor-core
        editor-operations
        sickle-gtk-lua
        se-lua
        textureselector
        PkgConfig::GTKMM
        # vvv temp vvv
        glutils::glutils # maparea3d
        world3d # maparea3d
        LuaConsole
        LuaWindow
    PRIVATE
        map
        rmf
        luageo
)
target_include_directories(appwin PUBLIC .)