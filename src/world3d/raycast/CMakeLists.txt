add_library(world3d-raycast STATIC
    BoxCollider.cpp
    BoxColliderBrush.cpp
    BoxColliderPointEntity.cpp
    ColliderFactory.cpp
)
target_include_directories(world3d-raycast PRIVATE .)
target_link_libraries(world3d-raycast PUBLIC
    editor-core
    editor-interfaces
)