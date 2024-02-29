add_library(utils INTERFACE)

add_library(gtkglutils STATIC gtkglutils.cpp)
target_include_directories(gtkglutils PRIVATE .)
target_link_libraries(gtkglutils
    PUBLIC
        glutils::glutils
    PRIVATE
        config
        PkgConfig::GTKMM
)