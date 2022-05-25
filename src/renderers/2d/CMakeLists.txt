add_library(
  renderers2d
  line.cpp
  stylefactory.cpp
  base.h
  line.h
  stylefactory.h)

target_include_directories(
  renderers2d
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
