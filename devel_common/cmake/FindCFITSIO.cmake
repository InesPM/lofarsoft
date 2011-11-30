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

# - Check for the presence of the CFITSIO library
#
#  CFITSIO_FOUND          = Do we have CFITSIO?
#  CFITSIO_LIBRARIES     = Set of libraries required for linking against CFITSIO
#  CFITSIO_INCLUDES      = Directory where to find fitsio.h
#  CFITSIO_VERSION       = Full version of the CFITSIO library (as deduced from
#                          fitsio.h)
#  CFITSIO_MAJOR_VERSION = Major version of the CFITSIO library
#  CFITSIO_MINOR_VERSION = Minor version of the CFITSIO library

if (NOT FIND_CFITSIO_CMAKE)

  set (FIND_CFITSIO_CMAKE TRUE)

  ##_____________________________________________________________________________
  ## Search locations

  include (CMakeSettings)

  ##_____________________________________________________________________________
  ## Check for the header files

  set (CFITSIO_INCLUDES "")

  FIND_PATH (HAVE_FITSIO_H fitsio.h
    HINTS ${CFITSIO_ROOT_DIR}
    PATHS ${include_locations}
    PATH_SUFFIXES include include/cfitsio cfitsio cfitsio/include
#    NO_DEFAULT_PATH
    )

  if (HAVE_FITSIO_H)
    list (APPEND CFITSIO_INCLUDES ${HAVE_FITSIO_H})
  endif (HAVE_FITSIO_H)

  ##_____________________________________________________________________________
  ## Check for the parts of the library

  FIND_LIBRARY (CFITSIO_CFITSIO_LIBRARY
    NAMES cfitsio
    HINTS ${CFITSIO_ROOT_DIR}
    PATHS ${lib_locations}
    PATH_SUFFIXES cfitsio cfitsio/lib
#    NO_DEFAULT_PATH
    )

  if (CFITSIO_CFITSIO_LIBRARY)
    set (CFITSIO_LIBRARIES ${CFITSIO_CFITSIO_LIBRARY})
  endif (CFITSIO_CFITSIO_LIBRARY)

  ##_____________________________________________________________________________
  ## Determine library version

  if (HAVE_FITSIO_H)

    ## parse the header file for version string
    file (STRINGS ${HAVE_FITSIO_H}/fitsio.h CFITSIO_VERSION
      REGEX "CFITSIO_VERSION"
      )

    if (CFITSIO_VERSION)
      ## extract full library version
      string (REGEX REPLACE "#define CFITSIO_VERSION " "" CFITSIO_VERSION ${CFITSIO_VERSION})
      ## exctract the major version of the library
      string(SUBSTRING ${CFITSIO_VERSION} 0 1 CFITSIO_MAJOR_VERSION)
      ## extract the minor version fo the library
      string(REGEX REPLACE "${CFITSIO_MAJOR_VERSION}." "" CFITSIO_MINOR_VERSION ${CFITSIO_VERSION})
    else (CFITSIO_VERSION)
      message (STATUS "Unable to extract CFITSIO version!")
    endif (CFITSIO_VERSION)

  endif (HAVE_FITSIO_H)

  ##_____________________________________________________________________________
  ## Check libcfitsio for required symbols

  include (CheckLibraryExists)

  if (CFITSIO_CFITSIO_LIBRARY)
    foreach (libsymbol file_open ftp_open file_openfile ffopen ffopentest ffreopen)
      check_library_exists (
	${CFITSIO_CFITSIO_LIBRARY}
	${libsymbol}
	""
	libcfitsio_has_${libsymbol}
	)
    endforeach (libsymbol)
  endif (CFITSIO_CFITSIO_LIBRARY)

  ##_____________________________________________________________________________
  ## Actions taken when all components have been found

  IF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
    SET (CFITSIO_FOUND TRUE)
  ELSE (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)
    SET (CFITSIO_FOUND FALSE)
    IF (NOT CFITSIO_FIND_QUIETLY)
      IF (NOT CFITSIO_INCLUDES)
        MESSAGE (STATUS "Unable to find CFITSIO header files!")
      ENDIF (NOT CFITSIO_INCLUDES)
      IF (NOT CFITSIO_LIBRARIES)
        MESSAGE (STATUS "Unable to find CFITSIO library files!")
      ENDIF (NOT CFITSIO_LIBRARIES)
    ENDIF (NOT CFITSIO_FIND_QUIETLY)
  ENDIF (CFITSIO_INCLUDES AND CFITSIO_LIBRARIES)

  IF (CFITSIO_FOUND)
    IF (NOT CFITSIO_FIND_QUIETLY)
      MESSAGE (STATUS "Found components for CFITSIO")
      MESSAGE (STATUS "CFITSIO_LIBRARIES     = ${CFITSIO_LIBRARIES}")
      MESSAGE (STATUS "CFITSIO_INCLUDES      = ${CFITSIO_INCLUDES}")
      message (STATUS "CFITSIO full version  = ${CFITSIO_VERSION}")
      message (STATUS "CFITSIO major version = ${CFITSIO_MAJOR_VERSION}")
      message (STATUS "CFITSIO minor version = ${CFITSIO_MINOR_VERSION}")
    ENDIF (NOT CFITSIO_FIND_QUIETLY)
  ELSE (CFITSIO_FOUND)
    IF (CFITSIO_FIND_REQUIRED)
      MESSAGE (FATAL_ERROR "Could not find CFITSIO!")
    ENDIF (CFITSIO_FIND_REQUIRED)
  ENDIF (CFITSIO_FOUND)

  set (HAVE_CFITSIO ${CFITSIO_FOUND})

  ##_____________________________________________________________________________
  ## Mark as advanced ...

  MARK_AS_ADVANCED (
    CFITSIO_CFITSIO_LIBRARY
    CFITSIO_INCLUDES
    CFITSIO_LIBRARIES
    CFITSIO_VERSION
    CFITSIO_MAJOR_VERSION
    CFITSIO_MINOR_VERSION
    HAVE_FITSIO_H
    )

  if (CFITSIO_libnsl)
    mark_as_advanced (
      CFITSIO_libnsl
      )
  endif (CFITSIO_libnsl)

endif (NOT FIND_CFITSIO_CMAKE)
