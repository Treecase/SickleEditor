flex_target(maplexer
    maplexer.lpp
    "${CMAKE_CURRENT_BINARY_DIR}/MAPScanner.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/_MAPScanner.hpp"
    COMPILE_FLAGS -+
)
bison_target(mapparser
    mapparser.ypp
    "${CMAKE_CURRENT_BINARY_DIR}/MAPParser.cpp"
)
add_flex_bison_dependency(maplexer mapparser)

add_library(map_parsing OBJECT
    "${FLEX_maplexer_OUTPUTS}"
    "${BISON_mapparser_OUTPUTS}"
    MAPDriver.cpp
)
target_include_directories(map_parsing
    PRIVATE .. .
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${FLEX_INCLUDE_DIRS}"
)
target_link_libraries(map_parsing PRIVATE "${FLEX_LIBRARIES}" convexhull)