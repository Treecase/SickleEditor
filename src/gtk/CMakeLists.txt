
# Configure gtk info header.
configure_file(appid.hpp.in "${PROJECT_BINARY_DIR}/include/appid.hpp")

target_link_libraries(sickle PRIVATE sickle-gtk-lua PkgConfig::GTKMM)

add_subdirectory(lua)
add_subdirectory(classes)
