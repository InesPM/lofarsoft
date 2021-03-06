
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the PYFITS
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (PYFITS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYFITS_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (PYFITS_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (PYFITS_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## System inspection

if (NOT PYFITS_VERBOSE_CONFIGURE)
  set (LAPACK_FIND_QUIETLY   YES )
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT PYFITS_VERBOSE_CONFIGURE)

include (FindPyFITS)
include (FindNumPy)
include (FindLAPACK)

if (NOT HAVE_PYTHON)
  message (WARNING "[PyFITS] Unable to build project - missing Python!")
endif (NOT HAVE_PYTHON)

if (NOT LAPACK_BLAS_LIBRARY)
  message (WARNING "[PyFITS] Unable to build project - missing BLAS library!")
endif (NOT LAPACK_BLAS_LIBRARY)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYFITS OR PYFITS_FORCE_BUILD)
  
  include (ExternalProject)

  set (PYFITS_VERSION     "2.3.1")
  set (PYFITS_DIRECTORY   "pyfits-${PYFITS_VERSION}")
  set (PYFITS_TAR_ARCHIVE "${PYFITS_DIRECTORY}.tar.gz")
  
  ## create "site.cfg" used later on

  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  file (WRITE ${PYFITS_BINARY_DIR}/site.cfg "[DEFAULT] \n")
  file (APPEND ${PYFITS_BINARY_DIR}/site.cfg "library_dirs = /usr/lib:/usr/local/lib:/usr/lib64:/usr/local/lib64:/sw/lib:${python_libdir} \n")

  ##__________________________________________________________________
  ## Build the project

  find_file (have_pyfits_tgz ${PYFITS_TAR_ARCHIVE} ${PYFITS_SOURCE_DIR})

  if (have_pyfits_tgz)
    ExternalProject_Add (pyfits
      PREFIX ${PYFITS_BINARY_DIR}
      DOWNLOAD_DIR ${PYFITS_BINARY_DIR}/download
      SOURCE_DIR ${PYFITS_BINARY_DIR}/source
      URL ${have_pyfits_tgz}
      URL_MD5 82e6919667aea99925db212d38670b38
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND true
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${PYFITS_BINARY_DIR}/source/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${PYFITS_BINARY_DIR}/source/setup.py install --prefix= --home=${CMAKE_INSTALL_PREFIX}
      )
  else (have_pyfits_tgz)
    message (WARNING "[PyFITS] Unable to build project - missing source!")
  endif (have_pyfits_tgz)

  ##__________________________________________________________________
  ## Configuration summary
  
  if (PYFITS_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[NumPy] Summary of configuration status.")
    message (STATUS "BLAS_LIBRARIES   = ${BLAS_LIBRARIES}")
    message (STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
    message (STATUS "PYTHON_VERSION   = ${PYTHON_VERSION}")
    message ("-------------------------------------------------------------")
  endif (PYFITS_VERBOSE_CONFIGURE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYFITS OR PYFITS_FORCE_BUILD)
  add_custom_target (pyfits
    WORKING_DIRECTORY ${PYFITS_BINARY_DIR}
    COMMENT "[PyFITS] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_PYFITS OR PYFITS_FORCE_BUILD)
