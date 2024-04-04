add_library(wad STATIC load_wad.cpp lumps.cpp)
target_include_directories(wad PRIVATE .)
target_link_libraries(wad PUBLIC PkgConfig::sigc++)