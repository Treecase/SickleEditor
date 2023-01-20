
find_package(PkgConfig REQUIRED)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

add_library(editor STATIC Editor.cpp)
target_include_directories(editor PUBLIC include)
target_link_libraries(editor PUBLIC
    map
    PkgConfig::sigc++
)
