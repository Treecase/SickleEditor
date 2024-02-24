add_library(preferences STATIC PreferencesDialog.cpp WADList.cpp)
target_include_directories(preferences PRIVATE .)
target_link_libraries(preferences PUBLIC config PkgConfig::GTKMM)