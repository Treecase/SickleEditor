
configure_file(version.hpp.in "${PROJECT_BINARY_DIR}/include/version.hpp")

add_subdirectory(convexhull)
add_subdirectory(editor)
add_subdirectory(fgd)
add_subdirectory(gtk)
add_subdirectory(lua)
add_subdirectory(map)
add_subdirectory(rmf)
add_subdirectory(utils)
add_subdirectory(wad)
add_subdirectory(world3d)

target_sources(sickle PRIVATE main.cpp)
