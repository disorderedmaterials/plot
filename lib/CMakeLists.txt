qt_add_qml_module(
  unnamed
  URI com.projectdissolve
  VERSION 1.0
  SOURCES
  axis.h
  axis.cpp
  lineGeometry.cpp
  lineGeometry.h
  QML_FILES
  main.qml
  LineModel.qml
)

target_include_directories(
  unnamed
  PRIVATE ${PROJECT_SOURCE_DIR}/lib ${PROJECT_BINARY_DIR}/lib
          ${Qt6Widgets_INCLUDE_DIRS}
          ${Qt6Quick3D_INCLUDE_DIRS})

target_link_libraries(
  unnamed
  PUBLIC ${WHOLE_ARCHIVE_FLAG}
  PRIVATE # External libs
          Qt6::Widgets Qt6::Core Qt6::3DCore Qt6::3DRender Qt6::3DExtras Qt6::Quick)

set_target_properties(unnamed PROPERTIES PUBLIC_HEADER lineGeometry.h)

install(TARGETS unnamed)
