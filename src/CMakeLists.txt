# Meta-Objects
set(mildred_MOC_HDRS widget.h)
qt6_wrap_cpp(mildred_MOC_SRCS ${mildred_MOC_HDRS})

add_library(mildred ${mildred_MOC_SRCS} framegraph.cpp mouse.cpp widget.cpp framegraph.h version.h)

include_directories(mildred PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  mildred
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)

target_link_libraries(
  mildred
  PUBLIC ${WHOLE_ARCHIVE_FLAG} classes entities materials renderers1d
  PRIVATE # External libs
          Qt6::Widgets Qt6::Core Qt6::OpenGLWidgets Qt6::3DCore Qt6::3DRender
          Qt6::3DExtras)

add_subdirectory(classes)
add_subdirectory(entities)
add_subdirectory(materials)
add_subdirectory(renderers)
