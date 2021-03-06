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

# - Check for the presence of SIP
#
# The following variables are set when SIP is found:
#  HAVE_SIP       = Set to true, if all components of SIP have been found.
#  SIP_FOUND      = Set to true, if all components of SIP have been found.
#  SIP_INCLUDES   = Include path for the header files of SIP
#  SIP_LIBRARIES  = Link these to use SIP
#  SIP_VERSION    = Version of SIP.

if (NOT FIND_SIP_CMAKE)

  set (FIND_SIP_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## CMake common settings
  
  if (SIP_FIND_QUIETLY)
    set (PYTHON_FIND_QUIETLY YES )
  endif (SIP_FIND_QUIETLY)
  
  include (FindPython)
  
  ##_____________________________________________________________________________
  ## As the shared libraries of a Python module typically do not contain the 
  ## usual prefix, we need to remove it while searching for the Matplotlib libraries.
  ## In order however not to affect other CMake modules we need to swap back in the
  ## original prefixes once the end of this module is reached.
  
  set (TMP_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})
  set (CMAKE_FIND_LIBRARY_PREFIXES "")
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (SIP_INCLUDES sip.h
    PATHS ${include_locations}
    PATH_SUFFIXES
    python${PYTHON_VERSION}
    python${PYTHON_VERSION}/site-packages
    /Library/Frameworks/Python.framework/Versions/${PYTHON_VERSION}/bin/
    sip
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for the library
  
  find_library (SIP_LIBRARIES sip
    PATHS ${lib_locations}
    PATH_SUFFIXES
    python
    python${PYTHON_VERSION}
    python${PYTHON_VERSION}/site-packages
    sip
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for Python scripts
  
  find_file (SIP_SIPCONFIG_PY sipconfig.py
    PATHS ${lib_locations}
    PATH_SUFFIXES
    python
    python${PYTHON_VERSION}
    python${PYTHON_VERSION}/site-packages
    sip
    NO_DEFAULT_PATH
    )
  
  ##_____________________________________________________________________________
  ## Check for version of SIP
  
  if (SIP_SIPCONFIG_PY)
    file (STRINGS ${SIP_SIPCONFIG_PY} SIP_VERSION_STRING
      REGEX "sip_version_str"
      )
    ## extract the version string
    if (SIP_VERSION_STRING)
      ## check version string consisting of three digits
      string (REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" SIP_VERSION ${SIP_VERSION_STRING})
      ## check version string consisting of two digits
      if (NOT SIP_VERSION)
	string (REGEX MATCH "[0-9]+.[0-9]+" SIP_VERSION ${SIP_VERSION_STRING})
      endif (NOT SIP_VERSION)
    endif (SIP_VERSION_STRING)
  else (SIP_SIPCONFIG_PY)
    message (STATUS "[FindSIP] Unable to determine version - no config file!")
  endif (SIP_SIPCONFIG_PY)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (SIP_INCLUDES AND SIP_LIBRARIES)
    set (HAVE_SIP  TRUE)
    set (SIP_FOUND TRUE)
  else (SIP_INCLUDES AND SIP_LIBRARIES)
    set (HAVE_SIP  FALSE)
    set (SIP_FOUND FALSE)
    if (NOT SIP_FIND_QUIETLY)
      if (NOT SIP_INCLUDES)
	message (STATUS "Unable to find SIP header files!")
      endif (NOT SIP_INCLUDES)
      if (NOT SIP_LIBRARIES)
	message (STATUS "Unable to find SIP library files!")
      endif (NOT SIP_LIBRARIES)
    endif (NOT SIP_FIND_QUIETLY)
  endif (SIP_INCLUDES AND SIP_LIBRARIES)
  
  if (HAVE_SIP)
    if (NOT SIP_FIND_QUIETLY)
      message (STATUS "Found components for SIP")
      message (STATUS "SIP_INCLUDES  = ${SIP_INCLUDES}")
      message (STATUS "SIP_LIBRARIES = ${SIP_LIBRARIES}")
      message (STATUS "SIP_VERSION   = ${SIP_VERSION}")
    endif (NOT SIP_FIND_QUIETLY)
  else (HAVE_SIP)
    if (SIP_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find SIP!")
    endif (SIP_FIND_REQUIRED)
  endif (HAVE_SIP)
  
  ##_____________________________________________________________________________
  ## Mark advanced variables
  
  mark_as_advanced (
    SIP_INCLUDES
    SIP_LIBRARIES
    SIP_VERSION
    )
  
  ##_____________________________________________________________________________
  ## Reinstate the original value of CMAKE_FIND_LIBRARY_PREFIXES
  
  set (CMAKE_FIND_LIBRARY_PREFIXES ${TMP_FIND_LIBRARY_PREFIXES} CACHE STRING
    "Library prefixes"
    FORCE
    )
  
endif (NOT FIND_SIP_CMAKE)
