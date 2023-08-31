add_library(sickle-gtk-lua STATIC
    AppWin_Lua.cpp
    LuaGdkEvent.cpp
)
target_include_directories(sickle-gtk-lua PUBLIC .)
target_link_libraries(sickle-gtk-lua
    PUBLIC
        se-lua
    PRIVATE
        appwin
        wad
        PkgConfig::GTKMM
        glutils::glutils
)

add_subdirectory(MapArea2D)
add_subdirectory(MapArea3D)