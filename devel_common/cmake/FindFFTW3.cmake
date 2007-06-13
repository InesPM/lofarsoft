##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

# - Check for the presence of FFTW3
#
# The following variables are set when FFTW3 is found:
#  HAVE_FFTW3       = Set to true, if all components of FFTW3
#                          have been found.
#  FFTW3_INCLUDE_DIR = Include path for the header files of FFTW3
#  FFTW3_LIBRARY     = Link these to use FFTW3

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (FFTW3_INCLUDE_DIR fftw3.h
  PATHS /usr/local/include /usr/include /sw/include
  )

## -----------------------------------------------------------------------------
## Check for the library

FIND_LIBRARY (FFTW3_LIBRARY fftw3 fftw
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY)
  SET (HAVE_FFTW3 TRUE)
ELSE (FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY)
  IF (NOT FFTW3_FIND_QUIETLY)
    IF (NOT FFTW3_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find FFTW3 header files!")
    ENDIF (NOT FFTW3_INCLUDE_DIR)
    IF (NOT FFTW3_LIBRARY)
      MESSAGE (STATUS "Unable to find FFTW3 library files!")
    ENDIF (NOT FFTW3_LIBRARY)
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ENDIF (FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY)

IF (HAVE_FFTW3)
  IF (NOT FFTW3_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for FFTW3")
    MESSAGE (STATUS "FFTW3_INCLUDE_DIR = ${FFTW3_INCLUDE_DIR}")
    MESSAGE (STATUS "FFTW3_LIBRARY     = ${FFTW3_LIBRARY}")
  ENDIF (NOT FFTW3_FIND_QUIETLY)
ELSE (HAVE_FFTW3)
  IF (FFTW3_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find FFTW3!")
  ENDIF (FFTW3_FIND_REQUIRED)
ENDIF (HAVE_FFTW3)

## ------------------------------------------------------------------------------
## Mark as advanced ...

