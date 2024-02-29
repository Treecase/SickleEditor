add_library(config INTERFACE)
target_include_directories(config INTERFACE "${PROJECT_BINARY_DIR}/include")

configure_file(appid.hpp.in "${PROJECT_BINARY_DIR}/include/config/appid.hpp")
configure_file(
    version.hpp.in
    "${PROJECT_BINARY_DIR}/include/config/version.hpp")