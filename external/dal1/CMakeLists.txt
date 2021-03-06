## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (DAL1)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

##  Package information
set (DAL1_GIT_REPOSITORY https://github.com/nextgen-astrodata/DAL1.git CACHE STRING "DAL1 git repository.")

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )

  if (LUS_SOURCE_DIR)
    include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
  else (LUS_SOURCE_DIR)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_SOURCE_DIR)
endif (NOT LUS_SOURCE_DIR)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

include (ExternalProject)

## Build package from source
ExternalProject_Add (dal1
  GIT_REPOSITORY ${DAL1_GIT_REPOSITORY}
  GIT_TAG rename
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DDAL_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  PATCH_COMMAND patch -p0 -i ${PROJECT_SOURCE_DIR}/dal1-with-clang.patch
  )

add_dependencies (dal1 git casacore num_util)

