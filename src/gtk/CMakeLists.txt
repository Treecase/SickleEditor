target_link_libraries(sickle PRIVATE sickle-gtk-lua PkgConfig::GTKMM)

add_subdirectory(lua)
add_subdirectory(classes)
