
find_package(PkgConfig REQUIRED)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

add_library(editor STATIC
    Brush.cpp
    BrushBox.cpp
    Editor.cpp
    Entity.cpp
    Face.cpp
    Map.cpp
    Selection.cpp
)
target_include_directories(editor PUBLIC include)
target_link_libraries(editor PUBLIC
    convexhull map rmf
    PkgConfig::sigc++
)

add_subdirectory(lua)
