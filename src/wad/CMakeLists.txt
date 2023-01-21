
add_library(wad STATIC load_wad.cpp lumps.cpp)
target_include_directories(wad PUBLIC include)
