cmake_minimum_required(VERSION 3.15)
project(mildred)

set(DESCRIPTION "Qt3D example")
set(AUTHOR "Qt Project")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC OFF)

# Set policy for automatic linkage of Qt libs to project
cmake_policy(SET CMP0020 NEW)

# Request C++17 Standard Support (Required for std::optional and auto types in
# lambda parameters)
set(CMAKE_CXX_STANDARD 17)

if(APPLE)
  set(CMAKE_PREFIX_PATH "${QT_BASE_DIR}")
endif(APPLE)

find_package(OpenGL REQUIRED)
find_package(
  Qt6
  COMPONENTS Core
             Gui
             Widgets
             3DCore
             3DExtras
             3DRender
             3DExtras
  REQUIRED)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC OFF)

# Set policy for automatic linkage of Qt libs to project
cmake_policy(SET CMP0020 NEW)

# ------------------------------- #
# Perform platform-specific setup #
# ------------------------------- #

set(target_name mildred)

# -- Windows
if(WIN32)
  # Add defines for Windows systems - NOMINMAX to prevent conflicts with
  # std::min() and std::max() - NOGDI to prevent conflicts arising from Windows
  # defining ERROR as a global macro (used in ANTLR4)
  add_definitions(-DNOMINMAX -DNOGDI)
endif(WIN32)

# -- Unix
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(UNIX)

# -- OSX
if(APPLE)
  # Set some specific C++11 related options here (set_property below does not
  # seem to persist)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

  # Suppress warnings for undefined template vars
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undefined-var-template")

  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  add_definitions(-D_MAC)
endif(APPLE)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(SRCS src/)

# Process CMakeLists in subdirs
add_subdirectory(${SRCS})

# Add executable target(s)
add_executable(${target_name} MACOSX_BUNDLE ${SRCS}/main.cpp)

# Set project-local include directories for target
target_include_directories(
  ${target_name}
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${PROJECT_BINARY_DIR}/src
          ${Qt6Core_INCLUDE_DIRS}
          ${Qt6Gui_INCLUDE_DIRS}
          ${Qt6OpenGL_INCLUDE_DIRS}
          ${Qt6Widgets_INCLUDE_DIRS}
          ${Qt6OpenGL_INCLUDE_DIRS}
          ${Qt6OpenGLWidgets_INCLUDE_DIRS})

target_link_libraries(
  ${target_name}
  PUBLIC ${WHOLE_ARCHIVE_FLAG} main classes entities
  PRIVATE # External libs
          Qt6::Widgets Qt6::Core Qt6::3DCore Qt6::3DRender Qt6::3DExtras)
