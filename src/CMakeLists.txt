# User Interface Files
set(main_UIS mainwindow.ui)
qt6_wrap_ui(main_UIS_H ${main_UIS})

# Meta-Objects
set(main_MOC_HDRS mainwindow.h widget.h)
qt6_wrap_cpp(main_MOC_SRCS ${main_MOC_HDRS})

add_library(
  main
  ${main_UIS_H}
  ${main_MOC_SRCS}
  mainwindow_funcs.cpp
  framegraph.cpp
  widget.cpp
  main.cpp
  framegraph.h)

include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  main
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender Qt6::3DCore Qt6::3DExtras)

add_subdirectory(classes)
add_subdirectory(entities)
add_subdirectory(renderers)
