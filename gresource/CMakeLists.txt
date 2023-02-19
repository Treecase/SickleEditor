
# Generate the GResource XML file.
configure_file(sicklegtk.gresource.xml.in sicklegtk.gresource.xml)

# Build resources.cpp.
add_custom_command(
    OUTPUT resources.cpp
    COMMAND glib-compile-resources
        sicklegtk.gresource.xml
        --target=resources.cpp
        --sourcedir="${CMAKE_CURRENT_SOURCE_DIR}"
        --generate-source
)
add_library(resources OBJECT resources.cpp)
target_link_libraries(resources PRIVATE PkgConfig::GTKMM)

target_link_libraries(sickle PRIVATE resources)
