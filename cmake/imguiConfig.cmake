# Using config mode since we're getting imgui from a git submodule.

# Set config variables.
set(IMGUI_VERSION 1.89)
set(IMGUI_VERSION_MAJOR 1)
set(IMGUI_VERSION_MINOR 89)
message(STATUS "imgui: Version " ${IMGUI_VERSION})

# Underlying library.
add_library(imgui OBJECT)
target_sources(imgui PRIVATE
    extern/imgui/backends/imgui_impl_sdl.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
    extern/imgui/misc/cpp/imgui_stdlib.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
)
target_include_directories(imgui
    PUBLIC
    extern/imgui/
    extern/imgui/backends
)
target_link_libraries(imgui PUBLIC SDL2::SDL2)

# Namespaced alias.
add_library(imgui::imgui ALIAS imgui)
