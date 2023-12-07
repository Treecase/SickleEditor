find_package(PkgConfig REQUIRED)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

add_subdirectory(core)
add_subdirectory(interfaces)
add_subdirectory(lua)
add_subdirectory(operations)
add_subdirectory(world)