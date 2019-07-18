mitk_create_module(MyAwesomeLib
  DEPENDS MitkCore MitkQtWidgetsExt MitkQtWidgets MitkSceneSerialization MitkDiffusionCore MitkConnectomics MitkSegmentation MitkOpenCVVideoSupport
  WARNINGS_NO_ERRORS
  PACKAGE_DEPENDS VTK ITK Poco Qt5|Core
)

add_subdirectory(cmdapps)
