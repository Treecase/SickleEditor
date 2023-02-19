
add_library(sickle-gtk-lua STATIC
    AppWin_Lua.cpp
    LuaGdkEvent.cpp
)
target_include_directories(sickle-gtk-lua PUBLIC .)
target_link_libraries(sickle-gtk-lua PRIVATE editor)
target_link_libraries(sickle-gtk-lua PUBLIC se-lua)

# temp, needed by classes includes
target_include_directories(sickle-gtk-lua PRIVATE
    "${PROJECT_BINARY_DIR}/include")
target_link_libraries(sickle-gtk-lua PRIVATE
    PkgConfig::GTKMM
    glutils::glutils
    wad
)

add_subdirectory(MapArea2D)
