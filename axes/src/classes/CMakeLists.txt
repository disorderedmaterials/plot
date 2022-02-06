# Meta-Objects
set(classes_MOC_HDRS metrics.h)
qt6_wrap_cpp(classes_MOC_SRCS ${classes_MOC_HDRS})

add_library(classes ${classes_MOC_SRCS} cuboid.h cuboid.cpp metrics.cpp)

include_directories(classes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  classes
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)
