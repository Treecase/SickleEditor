
# Configure gtk info header.
configure_file(appid.hpp.in "${PROJECT_BINARY_DIR}/include/appid.hpp")

add_subdirectory(lua)
add_subdirectory(classes)
