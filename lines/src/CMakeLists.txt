add_library(
  main
  main.cpp
)

include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(
  main PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} Qt6::3DRender
              Qt6::3DCore Qt6::3DExtras
)
