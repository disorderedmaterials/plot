# Meta-Objects
set(mildred_MOC_HDRS widget.h)
qt6_wrap_cpp(mildred_MOC_SRCS ${mildred_MOC_HDRS})

# Shader Resources
set(shaders_RES_QRC shaders.qrc)
qt6_add_resources(shaders_RES ${shaders_RES_QRC})

add_library(
  mildred
  ${mildred_MOC_SRCS}
  ${shaders_RES}
  framegraph.cpp
  material.cpp
  mouse.cpp
  widget.cpp
  framegraph.h
  material.h
  version.h)

include_directories(mildred PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  mildred
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)

target_link_libraries(
  mildred
  PUBLIC ${WHOLE_ARCHIVE_FLAG} classes entities renderers1d
  PRIVATE # External libs
          Qt6::Widgets Qt6::Core Qt6::3DCore Qt6::3DRender Qt6::3DExtras
          OpenGL::GL)

set_target_properties(mildred PROPERTIES PUBLIC_HEADER widget.h)

install(TARGETS mildred)

add_subdirectory(classes)
add_subdirectory(entities)
add_subdirectory(renderers)
