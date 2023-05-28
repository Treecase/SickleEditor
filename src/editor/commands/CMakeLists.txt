
add_library(commands STATIC
    Commands.cpp
)
target_link_libraries(commands PUBLIC editor PRIVATE convexhull)
