add_library(wad STATIC LumpTexture.cpp WADReader.cpp)
target_include_directories(wad PRIVATE .)
