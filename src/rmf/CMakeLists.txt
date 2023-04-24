
add_library(rmf STATIC rmf.cpp)
target_include_directories(rmf PUBLIC include)
target_compile_features(rmf PUBLIC cxx_std_17)
