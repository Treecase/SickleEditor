find_package(Lua 5.4 REQUIRED)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

add_library(se-lua STATIC
    ReferenceManager.cpp
    function.cpp
    se-lua.cpp
)
target_include_directories(se-lua PUBLIC ${LUA_INCLUDE_DIR} PRIVATE .)
target_link_libraries(se-lua PUBLIC
    utils
    PkgConfig::sigc++
    PkgConfig::GTKMM
    ${LUA_LIBRARIES}
)
target_compile_features(se-lua PUBLIC cxx_std_17)

add_subdirectory(lua-geo)