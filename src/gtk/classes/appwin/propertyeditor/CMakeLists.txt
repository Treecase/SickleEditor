add_library(appwin-propertyeditor STATIC
    CellRendererProperty.cpp
    PropertyEditor.cpp
    cellrenderers/CellRendererColor.cpp
    cellrenderers/CellRendererFile.cpp
    cellrenderers/CellRendererFlags.cpp
    cellrenderers/Choices.cpp
    cellrenderers/Color255.cpp
    cellrenderers/File.cpp
    cellrenderers/Flags.cpp
    cellrenderers/Integer.cpp
    cellrenderers/String.cpp
)
target_link_libraries(appwin-propertyeditor
    PUBLIC
        editor-world
        PkgConfig::GTKMM
)
target_include_directories(appwin-propertyeditor PRIVATE .)