# Meta-Objects
set(mildred_MOC_HDRS)
qt6_wrap_cpp(mildred_MOC_SRCS ${mildred_MOC_HDRS})

# Shader Resources
set(shaders_RES_QRC )
qt6_add_resources(shaders_RES ${shaders_RES_QRC})

qt_add_qml_module(
  mildred
  URI
  com.projectdissolve
  VERSION
  1.0
  SOURCES
  shaders.qrc
  component.cpp
  component.h
  displaygroup.cpp
  displaygroup.h
  framegraph.cpp
  framegraph.h
  keyboard.cpp
  material.cpp
  material.h
  mouse.cpp
  scenegraph.cpp
  version.h)

include_directories(mildred PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  mildred
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)

target_link_libraries(
  mildred
  PUBLIC ${WHOLE_ARCHIVE_FLAG} entities renderers1d classes
  PRIVATE # External libs
          Qt6::Widgets Qt6::Core Qt6::3DCore Qt6::3DRender Qt6::3DExtras
          OpenGL::GL)

set_target_properties(mildred PROPERTIES PUBLIC_HEADER component.h)

install(TARGETS mildred)

add_subdirectory(classes)
add_subdirectory(entities)
add_subdirectory(renderers)
