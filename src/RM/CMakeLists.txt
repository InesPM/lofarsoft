
## Name of the project
project (RM)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

if (LUS_SOURCE_DIR)
  include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
else (LUS_SOURCE_DIR)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_SOURCE_DIR)

## =============================================================================
##
##  Options
##
## =============================================================================

option (RM_COMPILER_WARNINGS    "Enable standard set of compiler warnings?"  YES )
option (RM_DEBUGGING_MESSAGES   "Print debugging information?"               NO  )
option (RM_VERBOSE_CONFIGURE    "Verbose output during configuration?"       NO  )
option (RM_ENABLE_TESTING       "Build the test programs?"                   YES )
option (RM_WITH_ITPP            "Enable using IT++ library?"                 NO  )
option (RM_WITH_ARMADILLO       "Enable using Armadillo library?"            YES )
option (RM_OSX_ARCHITECTURES    "Set OS X build architectures"               NO  )

## =============================================================================
##
##  External dependencies
##
## =============================================================================

## Check functions -------------------------------

include (CheckFunctionExists)

check_function_exists (abs   HAVE_ABS  )
check_function_exists (fabs  HAVE_FABS )

## Standard CMake modules ------------------------

foreach (externalPackage
    Armadillo
    Boost
    CASACORE
    CFITSIO
    DAL1
    FFTW3
    GFortran
    GSL
    HDF5
    ITPP
    Motif
    LAPACK
    Python
    WCSLIB
    X11
    ZLIB
    )

  ## Get package name in uppercase
  string (TOUPPER ${externalPackage} varPackageUpper)

  ## Set verbosity level
  if (RM_VERBOSE_CONFIGURE)
    set (${externalPackage}_FIND_QUIETLY FALSE )
    set (${varPackageUpper}_FIND_QUIETLY FALSE )
  else (RM_VERBOSE_CONFIGURE)  
    set (${externalPackage}_FIND_QUIETLY TRUE )
    set (${varPackageUpper}_FIND_QUIETLY TRUE )
  endif (RM_VERBOSE_CONFIGURE)

  ## Load CMake module if package not yet located
  if (NOT ${varPackageUpper}_FOUND)
    find_package (${externalPackage})
  endif (NOT ${varPackageUpper}_FOUND)

endforeach (externalPackage)

if (RM_WITH_ITPP)
  find_package (ITPP)
endif (RM_WITH_ITPP)

## =============================================================================
##
##  Handling of configuration/build/install options
##
## =============================================================================

##____________________________________________________________________
## Handle option: Enable testing?

if (RM_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (RM_ENABLE_TESTING)

##____________________________________________________________________
## Handle option: debugging messages on/off

if (RM_DEBUGGING_MESSAGES)
  set (DEBUGGING_MESSAGES ON)
endif (RM_DEBUGGING_MESSAGES)

##____________________________________________________________________
## Handle option: Enable standard set of compiler warnings?  ON/OFF

if (RM_COMPILER_WARNINGS)
  set (CMAKE_CXX_FLAGS
    "-Wall -g -Wno-comment -Woverloaded-virtual -Wno-non-template-friend"
    )
endif (RM_COMPILER_WARNINGS)

##____________________________________________________________________
##                                            OS X build architectures

if (APPLE AND RM_OSX_ARCHITECTURES)
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
    set (CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "OS X build architectures" FORCE)
  else (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
      set (CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "OS X build architectures" FORCE)
    endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
endif (APPLE AND RM_OSX_ARCHITECTURES)

## =============================================================================
##
##  Installation
##
## =============================================================================

## No relinking and full RPATH for the install tree
## (http://www.cmake.org/Wiki/CMake_RPATH_handling)

# skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH TRUE)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

## =============================================================================
##
##  Location of (external) header files
##
## =============================================================================

## RM-Tools header files

include_directories (BEFORE
  ${RM_BINARY_DIR}
  ${RM_SOURCE_DIR}
  ${RM_SOURCE_DIR}/implement
  ${RM_SOURCE_DIR}/implement/rmLib
  ${RM_SOURCE_DIR}/implement/rmClean
  ${RM_SOURCE_DIR}/implement/rmSim
  ${RM_SOURCE_DIR}/implement/rmSynthesis
  ${RM_SOURCE_DIR}/implement/rmWiener
  ${RM_SOURCE_DIR}/apps/rmcmd
  ${RM_SOURCE_DIR}/test
  )

## System header files

if (HAVE_SYS_SYSCTL_H)
  add_definitions (-DHAVE_SYS_SYSCTL_H)
endif (HAVE_SYS_SYSCTL_H)

## External header files

if (RM_WITH_ARMADILLO)
  if (HAVE_ARMADILLO OR ARMADILLO_FOUND)
    include_directories (${ARMADILLO_INCLUDES})
    add_definitions (-DHAVE_ARMADILLO)
  endif (HAVE_ARMADILLO OR ARMADILLO_FOUND)
endif (RM_WITH_ARMADILLO)

if (HAVE_DAL1)
  include_directories (${DAL1_INCLUDES})
  ## additional compiler flags
  if (CASACORE_INCLUDES)
    set (WITH_CASA YES)
    add_definitions (-DWITH_CASA)
  endif (CASACORE_INCLUDES)
endif (HAVE_DAL1)

if (CASACORE_INCLUDES)
  include_directories (${CASACORE_INCLUDES})
  add_definitions (-DHAVE_CASA)
else (CASACORE_INCLUDES)
  message (STATUS "[RM] Missing casacore header files!")
endif (CASACORE_INCLUDES)

if (CFITSIO_INCLUDES)
  include_directories (${CFITSIO_INCLUDES})
  add_definitions (-DHAVE_CFITSIO)
else (CFITSIO_INCLUDES)
  message (STATUS "[RM] Missing CFITSIO header files!")
endif (CFITSIO_INCLUDES)

if (FFTW3_INCLUDES)
  include_directories (${FFTW3_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
else (FFTW3_INCLUDES)
  message (STATUS "[RM] Missing FFTW3 header files!")
endif (FFTW3_INCLUDES)

if (HDF5_INCLUDES)
  include_directories (${HDF5_INCLUDES})
  add_definitions (-DHAVE_HDF5)
else (HDF5_INCLUDES)
  message (STATUS "[RM] Missing HDF5 header files!")
endif (HDF5_INCLUDES)

if (RM_WITH_ITPP AND ITPP_FOUND)
  include_directories (${ITPP_INCLUDES})
  add_definitions (-DHAVE_ITPP)
endif (RM_WITH_ITPP AND ITPP_FOUND)

if (HAVE_WCSLIB OR WCSLIB_FOUND)
  include_directories (${WCSLIB_INCLUDES})
  add_definitions (-DHAVE_WCSLIB)
endif (HAVE_WCSLIB OR WCSLIB_FOUND)

## =============================================================================
##
##  External libraries
##
## =============================================================================

set (rm_link_libraries
  ${DAL1_LIBRARIES}
  ${CASA_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  ${GSL_LIBRARIES}
  )

if (ARMADILLO_LIBRARIES)
  list (APPEND rm_link_libraries ${ARMADILLO_LIBRARIES})
endif (ARMADILLO_LIBRARIES)

if (ITPP_LIBRARIES)
  list (APPEND rm_link_libraries ${ITPP_LIBRARIES})
endif (ITPP_LIBRARIES)

if (HDF5_LIBRARIES)
  list (APPEND rm_link_libraries ${HDF5_LIBRARIES})
endif (HDF5_LIBRARIES)

if (FFTW3_LIBRARIES)
  list (APPEND rm_link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (HAVE_LIBZ)
  list (APPEND rm_link_libraries ${HAVE_LIBZ})
endif (HAVE_LIBZ)

if (HAVE_LIBDL)
  list (APPEND rm_link_libraries ${HAVE_LIBDL})
endif (HAVE_LIBDL)

## =============================================================================
##
##  Configuration for the subdirectories
##
## =============================================================================

add_subdirectory (implement)
add_subdirectory (test)
add_subdirectory (apps)

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

message (STATUS "+============================================================+")
message (STATUS "| [RM-Tools] Summary of configuration settings               |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " System configuration:"                                        )
message (STATUS " .. Processor type ......... = ${CMAKE_SYSTEM_PROCESSOR}"      )
message (STATUS " .. CMake executable ....... = ${CMAKE_COMMAND}"               )
message (STATUS " .. CMake version .......... = ${CMAKE_VERSION}"               )
message (STATUS " .. System name ............ = ${CMAKE_SYSTEM}"                )
message (STATUS " .. C++ compiler ........... = ${CMAKE_CXX_COMPILER}"          )
message (STATUS " .. C compiler ............. = ${CMAKE_C_COMPILER}"            )
message (STATUS " .. size(void*) ............ = ${CMAKE_SIZEOF_VOID_P}"         )
message (STATUS " External packages:"                                           )
message (STATUS " .. Armadillo .............. = ${ARMADILLO_FOUND} "            )
message (STATUS " .. Boost++ ................ = ${BOOST_FOUND}"                 )
message (STATUS " .. CFITSIO ................ = ${CFITSIO_FOUND}"               )
message (STATUS "    .. CFITSIO include dir   = ${CFITSIO_INCLUDES}"            )
message (STATUS "    .. CFITSIO library       = ${CFITSIO_LIBRARIES}"           )
message (STATUS " .. casacore ............... = ${CASACORE_FOUND}"              )
message (STATUS " .. FFTW3 .................. = ${FFTW3_FOUND}"                 )
message (STATUS " .. HDF5 Library ........... = ${HDF5_FOUND}"                  )
message (STATUS " .. IT++ Library ........... = ${ITPP_FOUND}"                  )
message (STATUS " .. Python ................. = ${PYTHON_FOUND}"                )
message (STATUS "    .. Python version        = ${PYTHON_VERSION}"              )
message (STATUS "    .. Python interpreter    = ${PYTHON_EXECUTABLE}"           )
message (STATUS "    .. Python library        = ${PYTHON_LIBRARIES}"            )
message (STATUS " .. WCSLIB ................. = ${WCSLIB_FOUND}"                )
message (STATUS "    .. WCSLIB include dir    = ${WCSLIB_INCLUDES}"             )
message (STATUS "    .. WCSLIB library        = ${WCSLIB_LIBRARIES}"            )
message (STATUS "+============================================================+")
