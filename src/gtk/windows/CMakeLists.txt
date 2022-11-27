
add_custom_target(
    resources.res
    ALL
    COMMAND
        windres
        "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc"
        -O coff
        -o resources.res
    DEPENDS resources.rc
)
target_link_libraries(sicklegtk PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/resources.res"
)
