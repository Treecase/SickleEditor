# Generate the GResource XML file.
configure_file(sicklegtk.gresource.xml.in sicklegtk.gresource.xml)

# Build resources.cpp.
add_custom_command(
    OUTPUT resources.cpp
    COMMAND glib-compile-resources
        sicklegtk.gresource.xml
        --target=resources.cpp
        --sourcedir="${CMAKE_CURRENT_SOURCE_DIR}"
        --generate-source
    DEPENDS
        gtk/menus.ui
        gtk/OperationSearch.glade
        gtk/TextureSelector.glade
        icons/outliner/brush.png
        icons/outliner/entity.png
        icons/outliner/face.png
        lua/gdkevents.lua
        lua/gdkkeysyms.lua
        lua/gdktypes.lua
        shaders/billboard.vert
        shaders/map.frag
        shaders/map.vert
        shaders/PointEntityBox.frag
        shaders/transparent.frag
        logo.png
        logo.svg
        LuaConsole.css
        MapArea2D.css
)
add_library(resources OBJECT resources.cpp)
target_link_libraries(resources PRIVATE PkgConfig::GTKMM)

target_link_libraries(sickle PRIVATE resources)