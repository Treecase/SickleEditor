add_library(LuaWindow STATIC
    CallStackInspector.cpp
    FunctionInspector.cpp
    LuaConsole.cpp
    LuaDebugger.cpp
    LuaWindow.cpp
    StackInspector.cpp
)
target_link_libraries(LuaWindow
    PUBLIC
        se-lua
        PkgConfig::GTKMM
    PRIVATE
        config
)
target_include_directories(LuaWindow PUBLIC .)