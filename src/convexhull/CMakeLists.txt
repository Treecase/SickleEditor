add_library(convexhull STATIC
    HalfPlane.cpp
    facet_enumerate.cpp
    vertex_enumerate.cpp
)
target_include_directories(convexhull PRIVATE .)
target_link_libraries(convexhull PUBLIC glm::glm)