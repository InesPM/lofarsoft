# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2011                                                        |
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

# - Check for the presence of IT++
#
# The following variables are set when IT++ is found:
#  ITPP_FOUND      = Set to true, if all components of IT++ have been found.
#  ITPP_INCLUDES   = Include path for the header files of IT++
#  ITPP_LIBRARIES  = Link these to use IT++
#  ITPP_LFLAGS     = Linker flags (optional)

if (NOT FIND_ITPP_CMAKE)
  
  set (FIND_ITPP_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (ITPP_INCLUDES itbase.h itcomm.h
    PATHS ${include_locations}
    PATH_SUFFIXES itpp
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (ITPP_LIBRARIES itpp
    PATHS ${lib_locations}
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (ITPP_INCLUDES AND ITPP_LIBRARIES)
    set (ITPP_FOUND TRUE)
  else (ITPP_INCLUDES AND ITPP_LIBRARIES)
    set (ITPP_FOUND FALSE)
    if (NOT ITPP_FIND_QUIETLY)
      if (NOT ITPP_INCLUDES)
	message (STATUS "Unable to find ITPP header files!")
      endif (NOT ITPP_INCLUDES)
      if (NOT ITPP_LIBRARIES)
	message (STATUS "Unable to find ITPP library files!")
      endif (NOT ITPP_LIBRARIES)
    endif (NOT ITPP_FIND_QUIETLY)
  endif (ITPP_INCLUDES AND ITPP_LIBRARIES)
  
  if (ITPP_FOUND)
    if (NOT ITPP_FIND_QUIETLY)
      message (STATUS "Found components for ITPP")
      message (STATUS "ITPP_INCLUDES  = ${ITPP_INCLUDES}")
      message (STATUS "ITPP_LIBRARIES = ${ITPP_LIBRARIES}")
    endif (NOT ITPP_FIND_QUIETLY)
  else (ITPP_FOUND)
    if (ITPP_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find ITPP!")
    endif (ITPP_FIND_REQUIRED)
  endif (ITPP_FOUND)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    ITPP_INCLUDES
    ITPP_LIBRARIES
    )
  
endif (NOT FIND_ITPP_CMAKE)
