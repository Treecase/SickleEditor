add_library(textureselector STATIC
    TextureImage.cpp
    TextureLoadingWorker.cpp
    TextureSelector.cpp
)
target_link_libraries(textureselector
    PUBLIC
        editor-textures
        PkgConfig::GTKMM
    PRIVATE
        config
)
target_include_directories(textureselector PRIVATE .)