add_library(
  renderers1d
  line.cpp
  error_base.cpp
  error_stick.cpp
  error_tee.cpp
  stylefactory.cpp
  symbol_base.cpp
  symbol_triangle.cpp
  base.h
  line.h
  none.h
  error_stick.h
  error_tee.h
  error_none.h
  stylefactory.h
  symbol_triangle.h)

target_include_directories(
  renderers1d
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
