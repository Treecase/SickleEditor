
configure_file(version.hpp.in "${PROJECT_BINARY_DIR}/include/version.hpp")
configure_file(common.hpp "${PROJECT_BINARY_DIR}/include/common.hpp")

add_subdirectory(bsp)
add_subdirectory(convexhull)
add_subdirectory(editor)
add_subdirectory(fgd)
add_subdirectory(gtk)
add_subdirectory(lua)
add_subdirectory(map)
add_subdirectory(mdl)
add_subdirectory(utils)
add_subdirectory(wad)

target_sources(sickle PRIVATE main.cpp)

target_link_libraries(sickle PRIVATE convexhull editor fgd se-lua map mapgl utils wad)
