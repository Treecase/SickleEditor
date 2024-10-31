configure_file(sicklegtk.desktop.in sicklegtk.desktop)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sicklegtk.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    RENAME "${APPLICATION_ID}.desktop"
)