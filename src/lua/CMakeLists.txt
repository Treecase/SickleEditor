
find_package(Lua REQUIRED)

add_library(se-lua STATIC se-lua.cpp)
target_include_directories(se-lua PUBLIC include ${LUA_INCLUDE_DIR})
target_link_libraries(se-lua PUBLIC utils ${LUA_LIBRARIES})
target_compile_features(se-lua PUBLIC cxx_std_17)
