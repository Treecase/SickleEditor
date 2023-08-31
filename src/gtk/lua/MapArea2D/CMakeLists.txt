target_sources(sickle-gtk-lua PRIVATE
    GrabbableBox_Lua.cpp
    MapArea2D_Lua.cpp
    Transform2D_Lua.cpp
)
target_include_directories(sickle-gtk-lua PUBLIC .)
target_link_libraries(sickle-gtk-lua PUBLIC maparea2d PRIVATE editor-lua)