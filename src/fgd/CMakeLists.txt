
add_library(fgd STATIC fgd.cpp parsing/FGDDriver.cpp)
target_include_directories(fgd PUBLIC export)
target_link_libraries(fgd PRIVATE fgd_parsing)

add_subdirectory(parsing)
