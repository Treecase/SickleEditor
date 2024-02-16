add_library(editor-interfaces STATIC
    Component.cpp
    EditorObject.cpp
    Selectable.cpp
)
target_include_directories(editor-interfaces PRIVATE .)
target_link_libraries(editor-interfaces PUBLIC PkgConfig::GTKMM)