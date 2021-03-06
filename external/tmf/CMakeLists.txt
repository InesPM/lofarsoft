## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (TMF)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

##  Package information
set (TMF_GIT "https://github.com/pschella/tmf.git")

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (TMF_FROM_RELEASE "Use release version instead of trunk?" YES)

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

## Find Python
include (FindPython)
include (ExternalProject)

## Build package from source
if (TMF_FROM_RELEASE)

  ExternalProject_Add (tmf
    GIT_REPOSITORY ${TMF_GIT}
    GIT_TAG release
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHONLIBS_FOUND=${HAVE_PYTHON} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE_PATH=${PYTHON_INCLUDES} -DPYTHON_SITE_PACKAGES=${CMAKE_INSTALL_PREFIX}/lib/python
    )

  add_dependencies (tmf git)

else (TMF_FROM_RELEASE)

  ExternalProject_Add (tmf
    GIT_REPOSITORY ${TMF_GIT}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHONLIBS_FOUND=${HAVE_PYTHON} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE_PATH=${PYTHON_INCLUDES} -DPYTHON_SITE_PACKAGES=${CMAKE_INSTALL_PREFIX}/lib/python
    )

  add_dependencies (tmf git)

endif (TMF_FROM_RELEASE)

