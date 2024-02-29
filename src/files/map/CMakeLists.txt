add_library(map STATIC load_map.cpp mapsaver.cpp)
target_include_directories(map PRIVATE .)
target_link_libraries(map
    PUBLIC
        glm::glm
    PRIVATE
        convexhull
        map_parsing
)
target_compile_features(map PUBLIC cxx_std_17)

add_subdirectory(parsing)