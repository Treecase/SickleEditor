# Meant to be used with MSVC.
# Using config mode since we're getting SDL2 locally.

# Find library directory.
file(GLOB SDL2_PACKAGE_DIR extern/SDL2-*)

add_library(SDL2 INTERFACE)
target_include_directories(SDL2 INTERFACE "${SDL2_PACKAGE_DIR}/include")
target_link_libraries(SDL2 INTERFACE "${SDL2_PACKAGE_DIR}/lib/x64/SDL2.lib")
add_library(SDL2::SDL2 ALIAS SDL2)

add_library(SDL2main INTERFACE)
target_include_directories(SDL2main INTERFACE "${SDL2_PACKAGE_DIR}/include")
target_link_libraries(SDL2main INTERFACE "${SDL2_PACKAGE_DIR}/lib/x64/SDL2main.lib")
add_library(SDL2::SDL2main ALIAS SDL2main)
