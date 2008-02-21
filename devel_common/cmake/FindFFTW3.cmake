##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

# - Check for the presence of FFTW3
#
# The following variables are set when FFTW3 is found:
#  HAVE_FFTW3       = Set to true, if all components of FFTW3 have been found.
#  FFTW3_INCLUDES   = Include path for the header files of FFTW3
#  FFTW3_LIBRARIES  = Link these to use FFTW3

set (include_locations
  ## local installation
  ./../release/include
  ./../../release/include
  ## system-wide installation
  /usr/include
  /usr/local/include
  /usr/X11R6/include
  /opt/include
  /opt/local/include
  /sw/include
  ## extra locations
  /opt/aips++/local/include
  /var/chroot/meqtrees/usr/include/fftw3.h
)

set (lib_locations
  ## local installation
  ./../release/lib
  ./../../release/lib
  ## system-wide installation
  /usr/local/lib
  /usr/lib
  /usr/X11R6/lib
  /opt/lib
  /opt/local/lib
  /sw/lib
  ## extra locations
  /opt/aips++/local/lib
  /var/chroot/meqtrees/usr/lib
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (FFTW3_INCLUDES fftw3.h
  PATHS ${include_locations}
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (FFTW3_LIBRARIES fftw3 fftw
  PATHS ${lib_locations}
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
  SET (HAVE_FFTW3 TRUE)
ELSE (FFTW3_INCLUDES AND FFTW3_LIBRARIES)
  SET (HAVE_FFTW3 FALSE)
  IF (NOT FFTW3_FIND_QUIETLY)
    IF (NOT FFTW3_INCLUDES)
      MESSAGE (STATUS "Unable to find FFTW3 header files!")
    ENDIF (NOT FFTW3_INCLUDES)
    IF (NOT FFTW3_LIBRARIES)
      MESSAGE (STATUS "Unable to find FFTW3 library files!")
    ENDIF (NOT FFTW3_LIBRARIES)
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ENDIF (FFTW3_INCLUDES AND FFTW3_LIBRARIES)

IF (HAVE_FFTW3)
  IF (NOT FFTW3_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for FFTW3")
    MESSAGE (STATUS "FFTW3_INCLUDES  = ${FFTW3_INCLUDES}")
    MESSAGE (STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ELSE (HAVE_FFTW3)
  IF (FFTW3_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find FFTW3!")
  ENDIF (FFTW3_FIND_REQUIRED)
ENDIF (HAVE_FFTW3)

## ------------------------------------------------------------------------------
## Mark as advanced ...

