set(CPP_FILES
  src/internal/my_awesomeproject_gameplugin_PluginActivator.cpp
  src/internal/Game_Estructuras.cpp
	src/internal/GamePlay.cpp
)

set(UI_FILES
  src/internal/GameControls.ui
	src/internal/GamePlay.ui

)

set(MOC_H_FILES
  src/internal/my_awesomeproject_gameplugin_PluginActivator.h
  src/internal/Game_Estructuras.h
	src/internal/GamePlay.h
)

# List of resource files that can be used by the plugin system without loading
# the actual plugin. For example, the icon that is typically displayed in the
# plugin view menu at the top of the application window.
set(CACHED_RESOURCE_FILES
  resources/AwesomeIcon.png
  resources/GameIcon.jpg
  plugin.xml
)

set(QRC_FILES
)
