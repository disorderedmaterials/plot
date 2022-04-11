# Resources
set(shaders_RES_QRC shaders.qrc)
qt6_add_resources(shaders_RES ${shaders_RES_QRC})

add_library(materials ${materials_MOC_SRCS} ${shaders_RES} monochrome_phong.h
                      monochrome_phong.cpp)

target_include_directories(
  materials
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt63DCore_INCLUDE_DIRS} ${Qt63DExtras_INCLUDE_DIRS}
          ${Qt63DRender_INCLUDE_DIRS})
