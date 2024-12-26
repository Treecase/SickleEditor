add_library(rmf STATIC rmf.cpp)
target_include_directories(rmf PRIVATE .)

## Uncomment to enable RMF loader debug logging.
# target_compile_definitions(rmf PRIVATE RMFENABLEDEBUG=1)