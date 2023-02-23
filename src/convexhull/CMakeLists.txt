
add_library(convexhull STATIC convexhull.cpp)
target_include_directories(convexhull PUBLIC include)
target_link_libraries(convexhull PUBLIC glm::glm)
