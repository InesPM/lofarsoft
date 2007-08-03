##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of the Boost library
#
#  HAVE_BOOST      = do we have BOOST?
#  BOOST_INCLUDES  = location of the include files
#  BOOST_LIBRARIES = location of the libraries
#

include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckTypeSize)

set (include_locations
  /include
  /usr/include
  /usr/local/include
  /opt/include
  /sw/include
  )

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (BOOST_INCLUDES config.hpp
  PATHS ${include_locations}
  PATH_SUFFIXES
  boost-1_33_1
  boost
  )

## -----------------------------------------------------------------------------
## Check for the various components of the library

## The libraries tend to come in different name variants, so we need to take this
## into account during the search. 

set (libs
  boost_date_time
  boost_filesystem
  boost_iostreams
  boost_program_options
  boost_python
  boost_regex
  boost_serialization
  boost_signals
  boost_test_exec_monitor
  boost_thread
  boost_unit_test_framework
  boost_wave
)

set (BOOST_LIBRARIES "")

foreach (lib ${libs})
  foreach (lib_variant ${lib}-gcc-1_33_1 ${lib}-gcc ${lib})
    ## try to locate the library
    find_library (BOOST_${lib_variant} ${lib_variant}
      PATHS ${lib_locations}
      PATH_SUFFIXES boost-1_33_1 boost
      )
    ## check if location was successful
    if (BOOST_${lib_variant})
      list (APPEND BOOST_LIBRARIES ${BOOST_${lib_variant}})
    endif (BOOST_${lib_variant})
  endforeach (lib_variant)
endforeach (lib)

## -----------------------------------------------------------------------------
## Check for symbols in the library
##
## We need this additional step especially for Python binding, as some of the
## required symbols might not be in place.

if (BOOST_boost_python-gcc-1_33_1)
  string (REGEX REPLACE libboost_python-gcc-1_33_1.so "" BOOST_LIBRARIES_DIR ${BOOST_boost_python-gcc-1_33_1})
endif (BOOST_boost_python-gcc-1_33_1)

if (BOOST_boost_python-gcc)
  string (REGEX REPLACE libboost_python-gcc.so "" BOOST_LIBRARIES_DIR ${BOOST_boost_python-gcc})
endif (BOOST_boost_python-gcc)

if (BOOST_LIBRARIES_DIR)
  ## check for symbols in the library
  check_library_exists (
    boost_python boost_python-gcc boost_python-gcc-1_33_1
    PyMem_Malloc
    ${BOOST_LIBRARIES_DIR}
    BOOST_PyMem_Malloc
    )
endif (BOOST_LIBRARIES_DIR)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (BOOST_INCLUDES AND BOOST_LIBRARIES)
  SET (HAVE_BOOST TRUE)
ELSE (BOOST_INCLUDES AND BOOST_LIBRARIES)
  IF (NOT Boost_FIND_QUIETLY)
    IF (NOT BOOST_INCLUDES)
      MESSAGE (STATUS "Unable to find Boost header files!")
    ENDIF (NOT BOOST_INCLUDES)
    IF (NOT BOOST_LIBRARIES)
      MESSAGE (STATUS "Unable to find Boost library files!")
    ENDIF (NOT BOOST_LIBRARIES)
  ENDIF (NOT Boost_FIND_QUIETLY)
ENDIF (BOOST_INCLUDES AND BOOST_LIBRARIES)

if (HAVE_BOOST)
  if (NOT Boost_FIND_QUIETLY)
    message (STATUS "Found components for Boost")
    message (STATUS "Boost library dir : ${BOOST_LIBRARIES_DIR}")
    message (STATUS "Boost library ... : ${BOOST_LIBRARIES}")
    message (STATUS "Boost headers ... : ${BOOST_INCLUDES}")
    message (STATUS "Have PyMem_Malloc : ${BOOST_PyMem_Malloc}")
  endif (NOT Boost_FIND_QUIETLY)
else (HAVE_BOOST)
  if (Boost_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find Boost!")
  endif (Boost_FIND_REQUIRED)
endif (HAVE_BOOST)

## -----------------------------------------------------------------------------
