add_library(textureselector STATIC
    TextureSelector.cpp
    TextureImage.cpp
)
target_link_libraries(textureselector
    PUBLIC
        editor-textures
        PkgConfig::GTKMM
    PRIVATE
        config
)
target_include_directories(textureselector PRIVATE .)