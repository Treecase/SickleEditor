
find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.2 REQUIRED)

flex_target(fgdlexer
    lexer.lpp
    "${CMAKE_CURRENT_BINARY_DIR}/FGDScanner.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/_FGDScanner.hpp"
    COMPILE_FLAGS -+
)
bison_target(fgdparser
    parser.ypp
    "${CMAKE_CURRENT_BINARY_DIR}/FGDParser.cpp"
)
add_flex_bison_dependency(fgdlexer fgdparser)

add_library(fgd_parsing OBJECT
    "${FLEX_fgdlexer_OUTPUTS}"
    "${BISON_fgdparser_OUTPUTS}"
    FGDDriver.cpp
)
target_include_directories(fgd_parsing PRIVATE
    "${FLEX_INCLUDE_DIRS}"
    ..
    .
)
target_include_directories(fgd_parsing PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
# target_link_libraries(fgd_parsing PRIVATE "${FLEX_LIBRARIES}")
target_compile_features(fgd_parsing PUBLIC cxx_std_17)
