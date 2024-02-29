add_library(luageo STATIC
    LuaGeo.cpp
    matrix.cpp
    vector.cpp
)
target_include_directories(luageo PRIVATE .)
target_link_libraries(luageo PUBLIC se-lua glm::glm)