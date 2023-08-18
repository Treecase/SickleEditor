add_library(world3d STATIC
    brush.cpp
    entity.cpp
    face.cpp
    texture.cpp
    vertex.cpp
    world3d.cpp
)
target_include_directories(world3d PUBLIC include)
target_link_libraries(world3d
    PUBLIC
        editor-core
        wad
        GLEW::GLEW
        glutils::glutils
        SickleGL
)
target_compile_features(world3d PUBLIC cxx_std_17)