add_library(
  renderers1d
  line.cpp
  error.cpp
  stylefactory.cpp
  base.h
  line.h
  error.h
  stylefactory.h)

target_include_directories(
  renderers1d
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
