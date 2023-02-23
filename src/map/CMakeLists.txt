
add_library(map STATIC load_map.cpp vertexmap.cpp)
target_include_directories(map PUBLIC include)
target_link_libraries(map PRIVATE convexhull PUBLIC glm::glm)
target_compile_features(map PUBLIC cxx_std_17)

add_library(mapgl STATIC glmap.cpp)
target_link_libraries(mapgl PUBLIC map wad glutils::glutils GLEW::GLEW SickleGL)
