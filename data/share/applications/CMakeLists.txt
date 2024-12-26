configure_file(SickleEditor.desktop.in ${APPLICATION_ID}.desktop)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${APPLICATION_ID}.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")