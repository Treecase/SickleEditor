
add_library(utils INTERFACE)
target_include_directories(utils INTERFACE .)

add_library(gtkglutils STATIC gtkglutils.cpp)
target_include_directories(gtkglutils PUBLIC .)
target_link_libraries(gtkglutils
    PUBLIC
        glutils::glutils
    PRIVATE
        config
        PkgConfig::GTKMM
)
