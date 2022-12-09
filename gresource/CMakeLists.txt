
# Generate the GResource XML file.
configure_file(sicklegtk.gresource.xml.in sicklegtk.gresource.xml)

# Generate resources.cpp.
# The touch is needed so target_sources can file a file with that name.
file(TOUCH "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp")
add_custom_target(resources.cpp
    COMMAND glib-compile-resources
        sicklegtk.gresource.xml
        --target=resources.cpp
        --sourcedir="${CMAKE_CURRENT_SOURCE_DIR}"
        --generate-source
)

add_dependencies(sickle resources.cpp)
target_sources(sickle PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp")
