add_library(spr STATIC spr.cpp)
target_include_directories(spr PRIVATE .)

add_executable(sprview sprview.cpp)
target_link_libraries(sprview PUBLIC spr PkgConfig::GTKMM)