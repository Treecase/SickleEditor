add_library(appwin-propertyeditor STATIC
    CellRendererProperty.cpp
    PropertyEditor.cpp
)
target_link_libraries(appwin-propertyeditor
    PUBLIC
        editor-world
        PkgConfig::GTKMM
)
target_include_directories(appwin-propertyeditor PRIVATE .)