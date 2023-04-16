
add_library(rmf STATIC rmf.cpp rmf2map.cpp)
target_include_directories(rmf PUBLIC include)
target_compile_features(rmf PUBLIC cxx_std_17)

target_link_libraries(rmf PRIVATE map) # temp
