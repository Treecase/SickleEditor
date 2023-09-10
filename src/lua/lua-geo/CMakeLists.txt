add_library(luageo STATIC
    LuaGeo.cpp
    matrix.cpp
    vector.cpp
)
target_include_directories(luageo PUBLIC .)
target_link_libraries(luageo PUBLIC se-lua glm::glm)