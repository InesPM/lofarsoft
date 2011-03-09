# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
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

# - Check for the presence of WCS
#
# The following variables are set when WCS is found:
#  WCSLIB_FOUND      = Set to true, if all components of WCS have been found.
#  WCSLIB_INCLUDES   = Include path for the header files of WCS
#  WCSLIB_LIBRARIES  = Link these to use WCS

if (NOT FIND_WCSLIB_CMAKE)

  set (FIND_WCSLIB_CMAKE TRUE  )
  set (WCSLIB_FOUND      FALSE )
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (WCSLIB_INCLUDES wcs/wcs.h wcslib/wcs.h
    PATHS ${include_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (WCSLIB_WCS_LIBRARY wcs
    PATHS ${lib_locations}
    PATH_SUFFIXES wcs wcslib darwin/lib linux_gnu/lib stable/linux_gnu/lib
    NO_DEFAULT_PATH
    )
  
  if (WCSLIB_WCS_LIBRARY)
    set (WCSLIB_LIBRARIES ${WCSLIB_WCS_LIBRARY})
  endif (WCSLIB_WCS_LIBRARY)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  IF (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)
    SET (WCSLIB_FOUND TRUE)
  ELSE (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)
    SET (WCSLIB_FOUND FALSE)
    IF (NOT WCSLIB_FIND_QUIETLY)
      IF (NOT WCSLIB_INCLUDES)
	MESSAGE (STATUS "Unable to find WCS header files!")
      ENDIF (NOT WCSLIB_INCLUDES)
      IF (NOT WCSLIB_LIBRARIES)
	MESSAGE (STATUS "Unable to find WCS library files!")
      ENDIF (NOT WCSLIB_LIBRARIES)
    ENDIF (NOT WCSLIB_FIND_QUIETLY)
  ENDIF (WCSLIB_INCLUDES AND WCSLIB_LIBRARIES)
  
  IF (WCSLIB_FOUND)
    IF (NOT WCSLIB_FIND_QUIETLY)
      MESSAGE (STATUS "Found components for WCS")
      MESSAGE (STATUS "WCSLIB_INCLUDES  = ${WCSLIB_INCLUDES}")
      MESSAGE (STATUS "WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")
    ENDIF (NOT WCSLIB_FIND_QUIETLY)
  ELSE (WCSLIB_FOUND)
    IF (WCSLIB_FIND_REQUIRED)
      MESSAGE (FATAL_ERROR "Could not find WCS!")
    ENDIF (WCSLIB_FIND_REQUIRED)
  ENDIF (WCSLIB_FOUND)
  
  set (HAVE_WCSLIB ${WCSLIB_FOUND})

  ##_____________________________________________________________________________
  ## Mark as advanced ...
  
  mark_as_advanced (
    WCSLIB_FOUND
    WCSLIB_INCLUDES
    WCSLIB_LIBRARIES
    WCSLIB_WCS_LIBRARY
    )
  
endif (NOT FIND_WCSLIB_CMAKE)
