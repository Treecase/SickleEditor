if(!WIN32)
    return()
endif()

find_program(WINDRES
    NAMES windres
    DOC "path to the windres program"
    REQUIRED)

add_custom_target(
    windows_resources
    COMMAND "${WINDRES}"
        -i resources.rc
        -o "${CMAKE_CURRENT_BINARY_DIR}/resources.o"
        -J rc
        -O coff
    DEPENDS resources.rc sickle.ico
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(sickle windows_resources)

target_link_libraries(sickle PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/resources.o")