##------------------------------------------------------------------------
## $Id:: template_FindXX.cmake 757 2007-09-12 16:07:55Z baehren          $
##------------------------------------------------------------------------

# Check for the presence of Data Access Library (DAL).
#
# The following variables are set when DAL is found:
#  HAVE_DAL       = Set to true, if all components of DAL have been found.
#  DAL_INCLUDES   = Include path for the header files of DAL
#  DAL_LIBRARIES  = Link these to use DAL
#  DAL_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  ./../release/bin
  ./../../release/bin
  /usr/local/bin
  /usr/bin
  /usr/X11R6/bin
  /opt/bin
  /opt/local/bin
  /sw/bin
  )

set (lib_locations
  ./../release/lib
  ./../../release/lib
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /opt/lib
  /opt/local/lib
  /sw/lib
  )

set (include_locations
  ./../release/include
  ./../../release/include
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (DAL_INCLUDES dal.h dalData.h dalFilter.h
  PATHS ${include_locations}
  PATH_SUFFIXES dal
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (DAL_LIBRARIES dal
  PATHS ${lib_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (DAL_INCLUDES AND DAL_LIBRARIES)
  set (HAVE_DAL TRUE)
else (DAL_INCLUDES AND DAL_LIBRARIES)
  set (HAVE_DAL FALSE)
  if (NOT DAL_FIND_QUIETLY)
    if (NOT DAL_INCLUDES)
      message (STATUS "Unable to find DAL header files!")
    endif (NOT DAL_INCLUDES)
    if (NOT DAL_LIBRARIES)
      message (STATUS "Unable to find DAL library files!")
    endif (NOT DAL_LIBRARIES)
  endif (NOT DAL_FIND_QUIETLY)
endif (DAL_INCLUDES AND DAL_LIBRARIES)

if (HAVE_DAL)
  if (NOT DAL_FIND_QUIETLY)
    message (STATUS "Found components for DAL")
    message (STATUS "DAL_INCLUDES  = ${DAL_INCLUDES}")
    message (STATUS "DAL_LIBRARIES = ${DAL_LIBRARIES}")
  endif (NOT DAL_FIND_QUIETLY)
else (HAVE_DAL)
  if (DAL_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find DAL!")
  endif (DAL_FIND_REQUIRED)
endif (HAVE_DAL)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  DAL_INCLUDES
  DAL_LIBRARIES
  )
