add_library(materials material.h material.cpp)

target_include_directories(
  materials
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
