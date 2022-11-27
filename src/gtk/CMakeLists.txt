
# Set Application ID
set(APPLICATION_ID com.github.treecase.sicklegtk)
# Set GResource prefix
set(GRESOURCE_PREFIX /com/github/treecase/sicklegtk/)
# Set GSchema path
set(SCHEMA_DIR "${CMAKE_CURRENT_BINARY_DIR}")


# Dependencies
# gtkmm3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)


# Configure files
# Desktop file
configure_file(sicklegtk.desktop.in ${APPLICATION_ID}.desktop)
# Settings schema
configure_file(sicklegtk.gschema.xml.in sicklegtk.gschema.xml)


# Generate gschemas.compiled.
add_custom_target(
    gschemas.compiled
    COMMAND
        glib-compile-schemas
        --strict
        --targetdir=.
        --schema-file=sicklegtk.gschema.xml
)

# Main executable target
add_executable(sicklegtk)
add_dependencies(sicklegtk gschemas.compiled)
target_compile_features(sicklegtk PRIVATE cxx_std_17)
set_target_properties(sicklegtk PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sicklegtk PRIVATE "${PROJECT_BINARY_DIR}/include")
target_link_libraries(sicklegtk PRIVATE PkgConfig::GTKMM)

# TODO: cleanup
target_link_libraries(sicklegtk PRIVATE
    glutils::glutils
    fgd
    map
    mapgl
    wad
)

# Set Windows .exe icon
if(WIN32)
    add_subdirectory(windows)
endif()

# Generate GResource data
add_subdirectory(gresource)

# Generate include headers, add source files
add_subdirectory(src)


# Install rules
include(GNUInstallDirs)
install(TARGETS sicklegtk)
# .desktop installed to `$XDG_DATA_DIRS/applications`
install(FILES "${PROJECT_BINARY_DIR}/${APPLICATION_ID}.desktop"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
)
# Icons installed to `$XDG_DATA_DIRS/icons/hicolor/48x48/apps/`
install(FILES gresource/logo.png
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/"
    RENAME ${APPLICATION_ID}.png
)
# Schema installed to `$XDG_DATA_DIRS/glib-2.0/schemas`
install(FILES "${PROJECT_BINARY_DIR}/sicklegtk.gschema.xml"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/glib-2.0/schemas"
    RENAME ${APPLICATION_ID}.gschema.xml
)
