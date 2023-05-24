
add_library(map STATIC load_map.cpp mapsaver.cpp)
target_include_directories(map PUBLIC include)
target_link_libraries(map PRIVATE convexhull map_parsing PUBLIC glm::glm)
target_compile_features(map PUBLIC cxx_std_17)

add_subdirectory(parsing)
