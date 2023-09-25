add_library(wad STATIC load_wad.cpp lumps.cpp TextureManager.cpp)
target_link_libraries(wad PUBLIC PkgConfig::sigc++)
target_include_directories(wad PUBLIC include)