configure_file(SickleEditor.gschema.xml.in ${APPLICATION_ID}.gschema.xml)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${APPLICATION_ID}.gschema.xml"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas")

# On Windows, GLib uses the install directory's share/ directory to source the
# GSettings schema. To facilitate this, we must compile the schema and install
# it alongside the gschema.xml file.
if(WIN32)
    find_program(GLIB_COMPILE_SCHEMAS
        NAMES glib-compile-schemas
        DOC "path to the glib-compile-schemas program"
        REQUIRED)

    add_custom_target(
        gschema
        COMMAND "${GLIB_COMPILE_SCHEMAS}" --strict "${CMAKE_CURRENT_BINARY_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    add_dependencies(sickle gschema)

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/gschemas.compiled"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas")
endif()