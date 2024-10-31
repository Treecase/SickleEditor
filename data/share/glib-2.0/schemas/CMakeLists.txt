configure_file(sicklegtk.gschema.xml.in sicklegtk.gschema.xml)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sicklegtk.gschema.xml"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas"
    RENAME "${APPLICATION_ID}.gschema.xml"
)

# TODO: This should run after installation.
# find_program(GLIB_COMPILE_SCHEMAS
#     NAMES glib-compile-schemas
#     DOC "path to the glib-compile-schemas program"
#     REQUIRED
# )
# install(CODE
#     "execute_process(
#         COMMAND \"${GLIB_COMPILE_SCHEMAS}\"
#         ARGS \"--strict\" \".\"
#         WORKING_DIRECTORY \"${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas\"
#     )"
# )