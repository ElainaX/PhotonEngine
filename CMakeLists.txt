cmake_minimum_required(VERSION 4.0)

project(Photon VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include(CMakeDependentOption)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

add_definitions(-DUNICODE -D_UNICODE)

set(PHOTON_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(Engine)