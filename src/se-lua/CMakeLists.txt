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

add_subdirectory(lua-geo)