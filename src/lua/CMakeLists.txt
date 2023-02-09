
find_package(Lua REQUIRED)

add_library(se-lua STATIC se-lua.cpp)
target_include_directories(se-lua PUBLIC include)
target_link_libraries(se-lua PUBLIC Lua)
