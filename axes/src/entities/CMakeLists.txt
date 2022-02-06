# Meta-Objects
set(entities_MOC_HDRS axis.h)
qt6_wrap_cpp(entities_MOC_SRCS ${entities_MOC_HDRS})

add_library(
  entities
  # Test UI
  ${entities_MOC_SRCS} axis.cpp line.cpp text.cpp line.h text.h)

include_directories(entities PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  entities
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)
