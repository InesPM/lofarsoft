# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

# - Check for the presence of NUMPY
#
# The following variables are set when NUMPY is found:
#  HAVE_NUMPY               = Set to true, if all components of NUMPY have been
#                             found.
#  NUMPY_INCLUDES           = Include path for the header files of NUMPY
#  NUMPY_LIBRARIES          = Link these to use NUMPY
#  MATPLOTLIB_LFLAGS             = Linker flags (optional)
#  MATPLOTLIB_API_VERSION        = API version of the installed and available Matplotlib
#                             package

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

## -----------------------------------------------------------------------------
## As the shared libraries of a Python module typically do not contain the 
## usual prefix, we need to remove it while searching for the Matplotlib libraries.
## In order however not to affect other CMake modules we need to swap back in the
## original prefixes once the end of this module is reached.

set (TMP_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})

set (CMAKE_FIND_LIBRARY_PREFIXES "" CACHE STRING
  "Library prefixes"
  FORCE
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (MATPLOTLIB_INCLUDES numerix.h mplutils.h
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python2.6/site-packages/matplotlib/core/include
  python2.5/site-packages/matplotlib/core/include
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (MATPLOTLIB_NXUTILS_LIBRARY NXUTILS
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python2.6/site-packages/matplotlib
  python2.5/site-packages/matplotlib
  python2.4/site-packages/matplotlib
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for executables

find_program (F2PY_EXECUTABLE f2py f2py2.6 f2py2.5 f2py2.4
  PATHS ${bin_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Try to determine the API version

execute_process (
  COMMAND python -c "import matplotlib; print matplotlib.__version__"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  RESULT_VARIABLE matplotlib_version_test_result
  OUTPUT_VARIABLE matplotlib_version_test_output
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

if (matplotlib_version_test_output)
  set (MATPLOTLIB_API_VERSION ${matplotlib_version_test_output})
endif (matplotlib_version_test_output)

include (FindPython)

if (MATPLOTLIB_INCLUDES)
find_file (MATPLOTLIB_NDARRAYOBJECT_H ndarrayobject.h
  PATHS ${MATPLOTLIB_INCLUDES}
  PATH_SUFFIXES matplotlib
  NO_DEFAULT_PATH
  )
endif (MATPLOTLIB_INCLUDES)

if (MATPLOTLIB_NDARRAYOBJECT_H)
  file (STRINGS ${MATPLOTLIB_NDARRAYOBJECT_H} NPY_VERSION
    REGEX "NPY_VERSION"
    )
  string (REGEX REPLACE "#define NPY_VERSION 0x" "" MATPLOTLIB_VERSION ${NPY_VERSION})
endif (MATPLOTLIB_NDARRAYOBJECT_H)

find_file (MATPLOTLIB_TEST_PROGRAM TestMatplotlibVersion.cc
  PATHS ${CMAKE_MODULE_PATH} ${USG_ROOT}
  PATH_SUFFIXES devel_common/cmake
  )

if (MATPLOTLIB_TEST_PROGRAM AND PYTHON_INCLUDES)
  ## try to compile and run
  try_run (
    MATPLOTLIB_TEST_RUN_RESULT
    MATPLOTLIB_TEST_COMPILE_RESULT
    ${CMAKE_BINARY_DIR}
    ${MATPLOTLIB_TEST_PROGRAM}
    COMPILE_DEFINITIONS -I${PYTHON_INCLUDES} -I${MATPLOTLIB_INCLUDES}
    COMPILE_OUTPUT_VARIABLE MATPLOTLIB_TEST_COMPILE
    RUN_OUTPUT_VARIABLE MATPLOTLIB_TEST_RUN
    )
  ## display results
  if (NOT MATPLOTLIB_FIND_QUIETLY)
    message (STATUS "MATPLOTLIB_TEST_RUN_RESULT     = ${MATPLOTLIB_TEST_RUN_RESULT}")
    message (STATUS "MATPLOTLIB_TEST_COMPILE_RESULT = ${MATPLOTLIB_TEST_COMPILE_RESULT}")
    message (STATUS "MATPLOTLIB_TEST_RUN            = ${MATPLOTLIB_TEST_RUN}")
  endif (NOT MATPLOTLIB_FIND_QUIETLY)
else (MATPLOTLIB_TEST_PROGRAM AND PYTHON_INCLUDES)
  message (STATUS "Unable to locate test program!")
endif (MATPLOTLIB_TEST_PROGRAM AND PYTHON_INCLUDES)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (MATPLOTLIB_INCLUDES AND MATPLOTLIB_LIBRARIES)
  set (HAVE_MATPLOTLIB TRUE)
else (MATPLOTLIB_INCLUDES AND MATPLOTLIB_LIBRARIES)
  set (HAVE_MATPLOTLIB FALSE)
  if (NOT MATPLOTLIB_FIND_QUIETLY)
    if (NOT MATPLOTLIB_INCLUDES)
      message (STATUS "Unable to find MATPLOTLIB header files!")
    endif (NOT MATPLOTLIB_INCLUDES)
    if (NOT MATPLOTLIB_LIBRARIES)
      message (STATUS "Unable to find MATPLOTLIB library files!")
    endif (NOT MATPLOTLIB_LIBRARIES)
  endif (NOT MATPLOTLIB_FIND_QUIETLY)
endif (MATPLOTLIB_INCLUDES AND MATPLOTLIB_LIBRARIES)

if (HAVE_MATPLOTLIB)
  if (NOT MATPLOTLIB_FIND_QUIETLY)
    message (STATUS "Found components for MATPLOTLIB")
    message (STATUS "MATPLOTLIB_INCLUDES  = ${MATPLOTLIB_INCLUDES}")
    message (STATUS "MATPLOTLIB_LIBRARIES = ${MATPLOTLIB_LIBRARIES}")
  endif (NOT MATPLOTLIB_FIND_QUIETLY)
else (HAVE_MATPLOTLIB)
  if (MATPLOTLIB_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find MATPLOTLIB!")
  endif (MATPLOTLIB_FIND_REQUIRED)
endif (HAVE_MATPLOTLIB)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  MATPLOTLIB_INCLUDES
  MATPLOTLIB_LIBRARIES
  MATPLOTLIB_MULTIARRAY_LIBRARY
  MATPLOTLIB_SCALARMATH_LIBRARY
  F2PY_EXECUTABLE
  MATPLOTLIB_API_VERSION
  )

## -----------------------------------------------------------------------------
## Reinstate the original value of CMAKE_FIND_LIBRARY_PREFIXES

set (CMAKE_FIND_LIBRARY_PREFIXES ${TMP_FIND_LIBRARY_PREFIXES} CACHE STRING
  "Library prefixes"
  FORCE
  )

