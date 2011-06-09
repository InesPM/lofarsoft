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

# - Check for the presence of ROOT
#
# The following variables are set when ROOT is found:
#  HAVE_ROOT       = Set to true, if all components of ROOT have been found.
#  ROOT_INCLUDES   = Include path for the header files of ROOT
#  ROOT_LIBRARIES  = Link these to use ROOT
#  ROOT_LFLAGS     = Linker flags (optional)

if (NOT FIND_ROOT_CMAKE)
  
  set (FIND_ROOT_CMAKE TRUE)
  
  ##_____________________________________________________________________________
  ## Search locations
  
  include (CMakeSettings)
  
  ##_____________________________________________________________________________
  ## Is the root of the installation defined through environment variable?
  
  set (HAVE_ROOTSYS $ENV{ROOTSYS})
  
  if (NOT ROOT_FIND_QUIETLY)
    if (HAVE_ROOTSYS)
      message (STATUS "Found environment variable ROOTSYS.")
    endif (HAVE_ROOTSYS)
  endif (NOT ROOT_FIND_QUIETLY)
  
  ##_____________________________________________________________________________
  ## Check for the header files
  
  find_path (ROOT_INCLUDES tCanvas.h TCint.h TObject.h
    PATHS
    ${include_locations}
    /opt
    /opt/auger/root-v5.18.00
    $ENV{ROOTSYS}
    $ENV{ROOTSYS}/include
    PATH_SUFFIXES
    include
    root
    root/include
    NO_DEFAULT_PATH
    )
  
  get_filename_component (ROOT_INCLUDES ${ROOT_INCLUDES} ABSOLUTE)
  
  ##_____________________________________________________________________________
  ## Check for the libraries
  
  set (libs
    ## output from 'root-config --glibs'
    Core
    Cint
    RIO
    Net
    Hist
    Graf
    Graf3d
    Gpad
    Tree
    Rint
    Postscript
    Matrix
    Minuit
    Physics
    MathCore
    Thread
    freetype
    Gui
    )
  
  foreach (lib ${libs})
    ## try to locate the library
    find_library (root${lib} ${lib}
      PATHS
      ${lib_locations}
      /opt/root/lib
      /opt/auger/root-v5.18.00
      $ENV{ROOTSYS}
      $ENV{ROOTSYS}/lib
      PATH_SUFFIXES
      root
      root/lib
      lib
      NO_DEFAULT_PATH
      )
    ## if the library could be located, it is added to the list
    if (root${lib})
      list (APPEND ROOT_LIBRARIES ${root${lib}})
    endif (root${lib})
  endforeach (lib)
  
  ## system libraries in addition to root libraries themselves
  
  find_library (libm m ${lib_locations})
  find_library (libdl dl ${lib_locations})
  
  if (libm)
    list (APPEND ROOT_LIBRARIES ${libm})
  endif (libm)
  
  if (libdl)
    list (APPEND ROOT_LIBRARIES ${libdl})
  endif (libdl)
  
  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  
  if (ROOT_INCLUDES AND ROOT_LIBRARIES)
    set (HAVE_ROOT TRUE)
  else (ROOT_INCLUDES AND ROOT_LIBRARIES)
    set (HAVE_ROOT FALSE)
    if (NOT ROOT_FIND_QUIETLY)
      if (NOT ROOT_INCLUDES)
	message (STATUS "Unable to find ROOT header files!")
      endif (NOT ROOT_INCLUDES)
      if (NOT ROOT_LIBRARIES)
	message (STATUS "Unable to find ROOT library files!")
      endif (NOT ROOT_LIBRARIES)
    endif (NOT ROOT_FIND_QUIETLY)
  endif (ROOT_INCLUDES AND ROOT_LIBRARIES)
  
  if (HAVE_ROOT)
    if (NOT ROOT_FIND_QUIETLY)
      message (STATUS "Found components for ROOT")
      message (STATUS "ROOT_INCLUDES  = ${ROOT_INCLUDES}")
      message (STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")
    endif (NOT ROOT_FIND_QUIETLY)
  else (HAVE_ROOT)
    if (ROOT_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find ROOT!")
    endif (ROOT_FIND_REQUIRED)
  endif (HAVE_ROOT)
  
  ##_____________________________________________________________________________
  ## Mark as advanced ...
  
  mark_as_advanced (
    ROOT_INCLUDES
    ROOT_LIBRARIES
    libm
    libdl
    )
  
  foreach (lib ${libs})
    if (root${lib})
      mark_as_advanced (root${lib})
    endif (root${lib})
  endforeach (lib)
  
endif (NOT FIND_ROOT_CMAKE)
