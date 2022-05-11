# Meta-Objects
set(classes_MOC_HDRS metrics.h)
qt6_wrap_cpp(classes_MOC_SRCS ${classes_MOC_HDRS})

add_library(classes ${classes_MOC_SRCS} colourdefinition.cpp cuboid.cpp
                    metrics.cpp colourdefinition.h cuboid.h)

target_include_directories(
  classes
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
