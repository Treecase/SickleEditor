
# Configure gtk info header.
configure_file(appid.hpp.in "${PROJECT_BINARY_DIR}/include/appid.hpp")


target_sources(sicklegtk PRIVATE main.cpp)

add_subdirectory(classes)
