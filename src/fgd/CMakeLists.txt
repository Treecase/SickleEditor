
add_library(fgd STATIC fgd.cpp)
target_link_libraries(fgd PRIVATE fgd_parsing)
target_include_directories(fgd PUBLIC include)

add_subdirectory(parsing)
