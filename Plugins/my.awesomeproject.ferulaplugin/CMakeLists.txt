project(my_awesomeproject_ferulaplugin)

mitk_create_plugin(
  EXPORT_DIRECTIVE AWESOME_EXPORTS
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PRIVATE MitkQtWidgetsExt MyAwesomeLib
  WARNINGS_NO_ERRORS

)
