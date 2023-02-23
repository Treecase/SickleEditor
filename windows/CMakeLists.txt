
add_custom_command(
    OUTPUT resources.res
    DEPENDS resources.rc sickle.ico
    COMMAND windres
        -i "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc"
        -O coff
        -o resources.res
)
add_library(windows-resources OBJECT resources.res)

target_link_libraries(sickle PRIVATE windows-resources)
