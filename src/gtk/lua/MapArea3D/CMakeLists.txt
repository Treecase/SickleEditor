
target_sources(sickle-gtk-lua PRIVATE
    FreeCam_Lua.cpp
    MapArea3D_Lua.cpp
    State_Lua.cpp
)
target_include_directories(sickle-gtk-lua PUBLIC .)
target_link_libraries(sickle-gtk-lua PRIVATE editor-lua luageo world3d)
