set(SOURCES
    "Include/Babylon/Plugins/CustomPlugin.h"
    "Source/CustomPlugin.cpp"
    "Source/CustomPluginClass.h"
    "Source/CustomPluginClass.cpp")

add_library(CustomPlugin ${SOURCES})

warnings_as_errors(CustomPlugin)

target_include_directories(CustomPlugin
    PUBLIC "Include")

target_link_to_dependencies(CustomPlugin
    PUBLIC napi
    PRIVATE arcana
    PRIVATE JsRuntime)

set_property(TARGET CustomPlugin PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})