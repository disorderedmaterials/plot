add_library(
  renderers1d
  line.cpp
  error_base.cpp
  error_stick.cpp
  error_tee.cpp
  stylefactory.cpp
  symbol_base.cpp
  symbol_triangle.cpp
  symbol_square.cpp
  symbol_diamond.cpp
  base.h
  line.h
  none.h
  error_stick.h
  error_tee.h
  error_none.h
  stylefactory.h
  symbol_none.h
  symbol_triangle.h
  symbol_square.h
  symbol_diamond.h)

target_include_directories(
  renderers1d
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
