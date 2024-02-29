add_library(rmf STATIC rmf.cpp)
target_include_directories(rmf PRIVATE .)
target_compile_features(rmf PUBLIC cxx_std_17)

## Uncomment to enable RMF loader debug logging.
# target_compile_definitions(rmf PRIVATE RMFENABLEDEBUG=1)