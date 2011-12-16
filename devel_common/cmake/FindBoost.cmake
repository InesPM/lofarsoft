# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |   **NB** Substantial changes by John Swinbank, July 2009.                   |
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

# - Check for the presence of the Boost library
#
#  HAVE_BOOST      = do we have BOOST?
#  BOOST_FOUND     = do we have BOOST?
#  BOOST_INCLUDES  = location of the include files
#  BOOST_LIBRARIES = location of the libraries
#

include (CheckIncludeFiles)
include (CheckTypeSize)

## -----------------------------------------------------------------------------
## Search locations

include (LUS_CMakeSettings)

## -----------------------------------------------------------------------------
## Check for the header files and the various module libraries

set (BOOST_FIND_python_ONLY NO)

set (BOOST_FIND_date_time           YES )
set (BOOST_FIND_filesystem          YES )
set (BOOST_FIND_iostreams           YES )
set (BOOST_FIND_program_options     YES )
set (BOOST_FIND_python              YES )
set (BOOST_FIND_regex               NO  )
set (BOOST_FIND_serialization       YES )
set (BOOST_FIND_signals             YES )
set (BOOST_FIND_system              YES )
set (BOOST_FIND_thread              YES )
set (BOOST_FIND_unit_test_framework YES )
set (BOOST_FIND_wave                YES )

if (BOOST_FIND_python_ONLY)
  if (NOT BOOST_FIND_QUIETLY)
    message (STATUS "[FindBoost] Configuration for boost_python only.")
  endif (NOT BOOST_FIND_QUIETLY)

  set (BOOST_FIND_python YES)

  set (BOOST_FIND_date_time           NO )
  set (BOOST_FIND_filesystem          NO )
  set (BOOST_FIND_iostreams           NO )
  set (BOOST_FIND_program_options     NO )
  set (BOOST_FIND_regex               NO )
  set (BOOST_FIND_serialization       NO )
  set (BOOST_FIND_signals             NO )
  set (BOOST_FIND_system              NO )
  set (BOOST_FIND_thread              NO )
  set (BOOST_FIND_unit_test_framework NO )
  set (BOOST_FIND_wave                NO )

endif (BOOST_FIND_python_ONLY)

set (boost_headers boost/config.hpp boost/version.hpp)

if (BOOST_FIND_date_time)
  list (APPEND boost_libraries boost_date_time)
  list (APPEND boost_headers boost/date_time.hpp)
endif (BOOST_FIND_date_time)

if (BOOST_FIND_filesystem)
  list (APPEND boost_libraries boost_filesystem)
  list (APPEND boost_headers boost/filesystem.hpp)
endif (BOOST_FIND_filesystem)

if (BOOST_FIND_iostreams)
  list (APPEND boost_libraries boost_iostreams)
endif (BOOST_FIND_iostreams)

if (BOOST_FIND_program_options)
  list (APPEND boost_libraries boost_program_options)
  list (APPEND boost_headers boost/program_options.hpp)
endif (BOOST_FIND_program_options)

if (BOOST_FIND_python)
  list (APPEND boost_libraries boost_python)
  list (APPEND boost_headers boost/python.hpp)
endif (BOOST_FIND_python)

if (BOOST_FIND_regex)
  list (APPEND boost_libraries boost_regex)
  list (APPEND boost_headers boost/regex.hpp)
endif (BOOST_FIND_regex)

if (BOOST_FIND_serialization)
  list (APPEND boost_libraries boost_serialization)
endif (BOOST_FIND_serialization)

if (BOOST_FIND_signals)
  list (APPEND boost_libraries boost_signals)
  list (APPEND boost_headers boost/signals.hpp)
endif (BOOST_FIND_signals)

if (BOOST_FIND_system)
  list (APPEND boost_libraries boost_system)
endif (BOOST_FIND_system)

if (BOOST_FIND_thread)
  list (APPEND boost_libraries boost_thread)
  list (APPEND boost_headers boost/thread.hpp)
endif (BOOST_FIND_thread)

if (BOOST_FIND_unit_test_framework)
  list (APPEND boost_libraries boost_unit_test_framework)
endif (BOOST_FIND_unit_test_framework)

if (BOOST_FIND_wave)
  list (APPEND boost_libraries boost_wave)
  list (APPEND boost_headers boost/wave.hpp)
endif (BOOST_FIND_wave)

## initialize list of detected libraries

set (BOOST_LIBRARIES "")
set (BOOST_INCLUDES  "")

foreach (boost_version 1_48_1 1_48_0 1_48 1_47_1 1_47_0 1_47 1_46 1_45 1_44) 

  ## Initialize book-keeping variable
  set (continue_search NO)
  
  ## <boost/config.hpp>
  foreach (hdr ${boost_headers})
    find_path (BOOST_INCLUDES_${hdr} ${hdr}
      PATH_SUFFIXES
      boost-${boost_version}
      boost
      .
      )
    if (BOOST_INCLUDES_${hdr})
      list (APPEND BOOST_INCLUDES_tmp ${BOOST_INCLUDES_${hdr}})
    else (BOOST_INCLUDES_${hdr})
      if (NOT BOOST_FIND_QUIETLY)
        message (STATUS "Not found: ${hdr} version ${boost_version}")
      endif (NOT BOOST_FIND_QUIETLY)
      set (continue_search YES)
    endif (BOOST_INCLUDES_${hdr})
  endforeach (hdr)
  ## Remove possible duplicates from the list of includes
  if (BOOST_INCLUDES_tmp)
    list (REMOVE_DUPLICATES BOOST_INCLUDES_tmp)
  endif (BOOST_INCLUDES_tmp)
  ## Determine the number of entries in the list
  list (LENGTH BOOST_INCLUDES_tmp num_includes)
  if (num_includes EQUAL 1)
    get_filename_component (BOOST_INCLUDES ${BOOST_INCLUDES_tmp} ABSOLUTE)
  else (num_includes EQUAL 1)
    if (NOT BOOST_FIND_QUIETLY)
      message (STATUS "Too many header directories for Boost ${boost_version}")
    endif (NOT BOOST_FIND_QUIETLY)
    set (continue_search YES)
  endif (num_includes EQUAL 1)

  ## Check for the module libraries --------------

  foreach (lib ${boost_libraries})
    ## convert library name to variable ("<NAME>_LIBRARY")
    string (TOUPPER ${lib} _lib)
    set (_lib_cache ${_lib}_LIBRARY)
    ## try to locate the library
    find_library (${_lib_cache}
      NAMES
      ${lib}
      ${lib}-gcc42-${boost_version}
      ${lib}-mt-${boost_version}
      ${lib}-gcc
      ${lib}-mt
      ${lib}-mt-d
      ${lib}-gcc41-mt-${boost_version}
      PATH_SUFFIXES
      boost
      boost-${boost_version}
      )
    ## check if location was successful
    if (${_lib_cache})
      list (APPEND BOOST_LIBRARIES ${${_lib_cache}})
    else (${_lib_cache})
      set (continue_search 1)
    endif (${_lib_cache})
  endforeach (lib)
  
  if (continue_search)
    if (NOT BOOST_FIND_QUIETLY)
      message (STATUS "Boost ${boost_version} not found")
    endif (NOT BOOST_FIND_QUIETLY)
    set (BOOST_LIBRARIES "")
    set (BOOST_INCLUDES  "")
  else (continue_search)
    break ()
  endif (continue_search)
  
endforeach (boost_version)

## -----------------------------------------------------------------------------
## Check for symbols in the library
##
## We need this additional step especially for Python binding, as some of the
## required symbols might not be in place.
##
## _NOTE_ This does not yet properly as 
##        "nm libboost_python-mt-1_34_1.dylib | grep _PyMem_Malloc"
##        report the symbol to be present in the library.

if (BOOST_boost_python)
  
  ## load CMake module required for checking symbols within a library
  include (CheckLibraryExists)

  check_library_exists (${BOOST_boost_python} PyMem_Malloc  "" BOOST__PyMem_Malloc)
  check_library_exists (${BOOST_boost_python} PyModule_Type "" BOOST__PyModule_Type)
  check_library_exists (${BOOST_boost_python} PyMethod_Type "" BOOST__PyMethod_Type)
  check_library_exists (${BOOST_boost_python} PyErr_WarnEx  "" BOOST__PyErr_WarnEx)

endif (BOOST_boost_python)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BOOST_INCLUDES AND BOOST_LIBRARIES)
  set (HAVE_BOOST  TRUE)
  set (BOOST_FOUND TRUE)
ELSE (BOOST_INCLUDES AND BOOST_LIBRARIES)
  set (HAVE_BOOST  FALSE)
  set (BOOST_FOUND FALSE)
  IF (NOT BOOST_FIND_QUIETLY)
    IF (NOT BOOST_INCLUDES)
      MESSAGE (STATUS "Unable to find Boost header files!")
    ENDIF (NOT BOOST_INCLUDES)
    IF (NOT BOOST_LIBRARIES)
      MESSAGE (STATUS "Unable to find Boost library files!")
    ENDIF (NOT BOOST_LIBRARIES)
  ENDIF (NOT BOOST_FIND_QUIETLY)
ENDIF (BOOST_INCLUDES AND BOOST_LIBRARIES)

if (HAVE_BOOST)
  if (NOT BOOST_FIND_QUIETLY)
    message (STATUS "Found components for Boost")
    ## list the components for which the search was enabled
    message (STATUS "date_time ......... : ${BOOST_FIND_date_time}")
    message (STATUS "filesystem ........ : ${BOOST_FIND_filesystem}")
    message (STATUS "iostreams ......... : ${BOOST_FIND_iostreams}")
    message (STATUS "program_options ... : ${BOOST_FIND_program_options}")
    message (STATUS "python ............ : ${BOOST_FIND_python}")
    message (STATUS "regex ............. : ${BOOST_FIND_regex}")
    message (STATUS "signals ........... : ${BOOST_FIND_signals}")
    message (STATUS "system ............ : ${BOOST_FIND_system}")
    message (STATUS "unit_test_framework : ${BOOST_FIND_unit_test_framework}")
    message (STATUS "thread ............ : ${BOOST_FIND_thread}")
    message (STATUS "wave .............. : ${BOOST_FIND_wave}")
    ## show search results
    message (STATUS "Boost library ..... : ${BOOST_LIBRARIES}")
    message (STATUS "Boost headers ..... : ${BOOST_INCLUDES}")
  endif (NOT BOOST_FIND_QUIETLY)
else (HAVE_BOOST)
  if (BOOST_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (BOOST_FIND_REQUIRED)
endif (HAVE_BOOST)

## Provide standard CMake variable

set (Boost_FOUND     ${HAVE_BOOST}      )
set (BOOST_FOUND     ${HAVE_BOOST}      )
set (Boost_LIBRARIES ${BOOST_LIBRARIES} )

## -----------------------------------------------------------------------------

mark_as_advanced (
  HAVE_BOOST
  BOOST_FOUND
  BOOST_INCLUDES
  BOOST_LIBRARIES
  Boost_FOUND
  Boost_LIBRARIES
)
