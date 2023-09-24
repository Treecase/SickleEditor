add_library(textureselector STATIC
    TextureSelector.cpp
    TextureImage.cpp
)
target_link_libraries(textureselector
    PUBLIC
        editor-core
        wad
        PkgConfig::GTKMM
    PRIVATE
        config
)
target_include_directories(textureselector PUBLIC .)