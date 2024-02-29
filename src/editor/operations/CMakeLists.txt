add_library(editor-operations STATIC
    Operation.cpp
    OperationLoader.cpp
)
target_include_directories(editor-operations PRIVATE .)
target_link_libraries(editor-operations
    PUBLIC
        se-lua
        PkgConfig::GTKMM
    PRIVATE
        editor-core
        editor-lua
        luageo
)