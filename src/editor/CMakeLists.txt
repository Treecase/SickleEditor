
find_package(PkgConfig REQUIRED)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

add_library(editor STATIC
    Brush.cpp
    BrushBox.cpp
    Editor.cpp
    Entity.cpp
    Face.cpp
    Map.cpp
    MapTools.cpp
    Operation.cpp
    Selection.cpp
)
target_include_directories(editor PUBLIC include)
target_link_libraries(editor
    PUBLIC
        convexhull
        map
        rmf
        se-lua
        PkgConfig::sigc++
    PRIVATE
        commands
)

add_subdirectory(lua)
add_subdirectory(commands)
