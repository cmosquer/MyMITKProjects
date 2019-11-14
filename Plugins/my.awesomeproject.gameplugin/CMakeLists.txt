project(my_awesomeproject_gameplugin)

mitk_create_plugin(
  EXPORT_DIRECTIVE AWESOME_EXPORTS
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PRIVATE MitkQtWidgetsExt
  WARNINGS_NO_ERRORS
  PACKAGE_DEPENDS  Qt5|Core+Multimedia
)
