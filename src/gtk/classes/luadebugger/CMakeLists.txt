add_library(LuaWindow STATIC
    CallStackInspector.cpp
    FunctionInspector.cpp
    LuaDebugger.cpp
    LuaWindow.cpp
    StackInspector.cpp
)
target_link_libraries(LuaWindow
    PUBLIC
        se-lua
        PkgConfig::GTKMM
)
target_include_directories(LuaWindow PUBLIC .)