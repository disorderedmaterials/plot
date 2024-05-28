set(target_name qml)

# Add executable target(s)
add_executable(${target_name} main.cpp ${${target_name}_MOC_SRCS})

# Set project-local include directories for target
target_include_directories(
  ${target_name}
  PRIVATE ${PROJECT_SOURCE_DIR}/lib
          ${PROJECT_BINARY_DIR}/lib
          ${PROJECT_BINARY_DIR}/examples/${target_name}
          ${PROJECT_SOURCE_DIR}/examples/${target_name}
          ${Qt6Qml_INCLUDE_DIRS}
          ${Qt6Core_INCLUDE_DIRS}
          ${Qt6Gui_INCLUDE_DIRS}
          ${Qt6OpenGL_INCLUDE_DIRS}
          ${Qt6Quick3D_INCLUDE_DIRS}
          ${Qt6Widgets_INCLUDE_DIRS})

target_link_libraries(${target_name} PRIVATE unnamed # External libs
                                             Qt6::Gui Qt6::Widgets Qt6::Qml Qt6::Core Qt6::Quick3D)

set_target_properties(${target_name} PROPERTIES RUNTIME ${target_name})

install(TARGETS ${target_name} RUNTIME)
