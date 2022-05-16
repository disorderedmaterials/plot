add_library(
  renderers1d
  line.cpp
  error_stick.cpp
  error_t_stick.cpp
  stylefactory.cpp
  base.h
  line.h
  line_none.h
  error_stick.h
  error_t_stick.h
  error_none.h
  stylefactory.h)

target_include_directories(
  renderers1d
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
