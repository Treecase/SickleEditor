add_library(editor-core-gamedefinition STATIC
    ClassPropertyFactory.cpp
    EntityClass.cpp
    EntityPropertyDefinition.cpp
    EntityPropertyDefinitionFactory.cpp
    GameDefinition.cpp
)
target_include_directories(editor-core-gamedefinition PRIVATE .)
target_link_libraries(editor-core-gamedefinition
    PUBLIC
        fgd
        glm::glm
)