# Meta-Objects
set(entities_MOC_HDRS axis.h data.h)
qt6_wrap_cpp(entities_MOC_SRCS ${entities_MOC_HDRS})

add_library(
  entities
  ${entities_MOC_SRCS} axis.cpp data.cpp data1d.cpp data2d.cpp data3d.cpp line.cpp text.cpp data1d.h data2d.h data3d.h line.h text.h)


target_include_directories(
  entities
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
        ${Qt63DCore_INCLUDE_DIRS}
        ${Qt63DExtras_INCLUDE_DIRS}
        ${Qt63DRender_INCLUDE_DIRS})
