
add_library(mdl STATIC load_model.cpp)
target_include_directories(mdl PUBLIC include)

add_library(mdlgl STATIC mdl2gl.cpp)
target_link_libraries(mdlgl PUBLIC mdl glutils::glutils)
