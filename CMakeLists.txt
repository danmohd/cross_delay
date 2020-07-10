
set(plug_sources 
    include/plugineditor.h
    include/pluginprocessor.h
    include/pluginids.h
    source/plugineditor.cpp
    source/pluginprocessor.cpp
    source/pluginfactory.cpp
    source/pluginfo.rc)

set(target DanishCrossDelay)

smtg_add_vst3plugin(${target} ${plug_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_MYPLUGINS_FOLDER})
target_link_libraries(${target} PRIVATE base sdk)