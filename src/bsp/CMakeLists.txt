
add_library(bsp STATIC entities_lump.cpp load_bsp.cpp)
target_include_directories(bsp PUBLIC include)

add_library(bspgl STATIC bsp2gl.cpp)
target_link_libraries(bspgl PUBLIC bsp glutils::glutils GLEW::GLEW SickleGL wad)
