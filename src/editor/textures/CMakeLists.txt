add_library(editor-textures STATIC
    TextureInfo.cpp
    TextureManager.cpp
)
target_include_directories(editor-textures PRIVATE .)
target_link_libraries(editor-textures PRIVATE wad PkgConfig::GTKMM)