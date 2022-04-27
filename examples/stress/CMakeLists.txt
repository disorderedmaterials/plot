set(target_name stress)
#
# User Interface Files set(x_UIs ) qt6_wrap_ui(x_UIS_H ${x_UIS})
#
# Meta-Objects
set(${target_name}_MOC_HDRS mainwindow.h)
qt6_wrap_cpp(${target_name}_MOC_SRCS ${${target_name}_MOC_HDRS})

# Add executable target(s)
add_executable(${target_name} main.cpp mainwindow.ui ${${target_name}_MOC_SRCS}
                              mainwindow_funcs.cpp)

# Set project-local include directories for target
target_include_directories(
  ${target_name}
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${PROJECT_BINARY_DIR}/src
          ${PROJECT_BINARY_DIR}/examples/${target_name}
          ${PROJECT_SOURCE_DIR}/examples/${target_name}
          ${Qt6Core_INCLUDE_DIRS}
          ${Qt6Gui_INCLUDE_DIRS}
          ${Qt6OpenGL_INCLUDE_DIRS}
          ${Qt6Widgets_INCLUDE_DIRS})

target_link_libraries(${target_name} PRIVATE # External libs
                                             mildred)

set_target_properties(${target_name} PROPERTIES RUNTIME ${target_name})

install(TARGETS ${target_name} RUNTIME)
