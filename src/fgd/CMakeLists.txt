
add_library(fgd STATIC fgd.cpp)
target_link_libraries(fgd PRIVATE fgd_parsing)
target_include_directories(fgd PUBLIC include)
target_compile_features(fgd PUBLIC cxx_std_17)

add_subdirectory(parsing)
