add_library(LuaConsole STATIC LuaConsole.cpp)
target_link_libraries(LuaConsole
    PUBLIC
        se-lua
        PkgConfig::GTKMM
    PRIVATE
        config
)
target_include_directories(LuaConsole PUBLIC .)