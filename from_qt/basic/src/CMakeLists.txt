# Meta-Objects
set(main_MOC_HDRS
  orbittransformcontroller.h
)
qt6_wrap_cpp(main_MOC_SRCS ${main_MOC_HDRS})

add_library(
  main
  main.cpp
  orbittransformcontroller_funcs.cpp
  ${main_MOC_SRCS}
)

include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  main PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender
              Qt6::3DCore Qt6::3DExtras
)
