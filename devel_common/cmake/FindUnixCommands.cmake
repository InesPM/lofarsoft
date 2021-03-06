# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2011                                                        |
# |   Lars B"ahren (lbaehren@gmail.com)                                         |
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

##____________________________________________________________________
## Platform-specific extension of the search locations

if (NOT UNIX)
  include (FindCygwin)
endif (NOT UNIX)

if (CYGWIN_INSTALL_PATH)
  list (INSERT CMAKE_PROGRAM_PATH 0 ${CYGWIN_INSTALL_PATH}/bin )
endif (CYGWIN_INSTALL_PATH)

##____________________________________________________________________

foreach (_unixCommand
    bash
    cp
    curl
    gzip
    hostname
    mv
    rm
    tar
    )
  
  ## Generate variable to store information
  string (TOUPPER ${_unixCommand} _varUnixCommand)
  
  ## Search for the executable
  if (NOT ${_varUnixCommand}_EXECUTABLE)
    find_program (${_varUnixCommand}_EXECUTABLE ${_unixCommand} )
  endif (NOT ${_varUnixCommand}_EXECUTABLE)
  
endforeach (_unixCommand)
