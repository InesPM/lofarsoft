
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (BLAS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## ------------------------------------------------------------------------------
## Check for location installation location

find_path (BLAS_INSTALL_PREFIX release_area.txt
  PATHS
  ${BLAS_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (BLAS_INSTALL_PREFIX)
  message (STATUS "Installation area located for package BLAS.")
  set (CMAKE_INSTALL_PREFIX ${BLAS_INSTALL_PREFIX}) 
endif (BLAS_INSTALL_PREFIX)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

include (FindBLAS)

if (NOT BLAS_LIBRARIES)
  find_library (BLAS_LIBRARIES blas)
endif (NOT BLAS_LIBRARIES)

## ==============================================================================
##
##  Package information
##
## ==============================================================================

set (BLAS_VERSION        "" )
set (BLAS_SOURCE_ARCHIVE "blas.tgz" )
set (BLAS_URL            "http://www.netlib.org/blas/${BLAS_SOURCE_ARCHIVE}" )
set (BLAS_SVN            "" )
set (BLAS_MD5            "7e6af7022440d8688d16be86d55fb358" )

set (CBLAS_VERSION        "" )
set (CBLAS_SOURCE_ARCHIVE "cblas.tgz" )
set (CBLAS_URL            "http://www.netlib.org/blas/blast-forum/${CBLAS_SOURCE_ARCHIVE}" )
set (CBLAS_SVN            "" )
set (CBLAS_MD5            "716329646ac933592a7cbdbeae9d3d06" )

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (NOT BLAS_LIBRARIES)
  
  ##____________________________________________________________________
  ##                                                                BLAS
  
  ## Source location
  
  find_file (HAVE_BLAS_SOURCE ${BLAS_SOURCE_ARCHIVE}
    PATHS ${BLAS_SOURCE_DIR}
    )
  
  if (NOT HAVE_BLAS_SOURCE)
    set (HAVE_BLAS_SOURCE ${BLAS_URL})
  endif (NOT HAVE_BLAS_SOURCE)
  
  ## Build and install package
  
  ExternalProject_Add (blas
    PREFIX ${BLAS_BINARY_DIR}
    DOWNLOAD_DIR ${BLAS_BINARY_DIR}/download
    SOURCE_DIR ${BLAS_BINARY_DIR}/blas
    URL ${HAVE_BLAS_SOURCE}
    CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    INSTALL_COMMAND make install
    )
  
  ExternalProject_Add_Step (blas blas_cmake
    COMMAND cp ${BLAS_SOURCE_DIR}/blas.cmake ${BLAS_BINARY_DIR}/blas/CMakeLists.txt
    COMMENT "Moving CMakeLists.txt into place ..."
    DEPENDEES patch
    DEPENDERS configure
    WORKING_DIRECTORY ${BLAS_BINARY_DIR}/blas
    )
  
  ##____________________________________________________________________
  ##                                                               CBLAS
  
  ## Source location
  
  find_file (HAVE_CBLAS_SOURCE ${CBLAS_SOURCE_ARCHIVE}
    PATHS ${BLAS_SOURCE_DIR}
    )
  
  if (NOT HAVE_CBLAS_SOURCE)
    set (HAVE_CBLAS_SOURCE ${CBLAS_URL})
  endif (NOT HAVE_CBLAS_SOURCE)
  
  ## Build and install package
  
  ExternalProject_Add (cblas
    PREFIX ${BLAS_BINARY_DIR}
    DOWNLOAD_DIR ${BLAS_BINARY_DIR}/download
    SOURCE_DIR ${BLAS_BINARY_DIR}/cblas
    URL ${HAVE_CBLAS_SOURCE}
    CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    INSTALL_COMMAND make install
    )
  
  ExternalProject_Add_Step (cblas cblas_cmake
    COMMAND cp ${BLAS_SOURCE_DIR}/cblas.cmake ${BLAS_BINARY_DIR}/cblas/CMakeLists.txt
    COMMENT "Moving CMakeLists.txt into place ..."
    DEPENDEES patch
    DEPENDERS configure
    WORKING_DIRECTORY ${BLAS_BINARY_DIR}/cblas
    )
  
  ## Package dependencies
  
  add_dependencies (cblas blas)
  
else (NOT BLAS_LIBRARIES)

    add_custom_target (blas
    WORKING_DIRECTORY ${BLAS_BINARY_DIR}
    COMMENT "[BLAS] Found system-wide installation; skipping rebuild!"
    )

endif (NOT BLAS_LIBRARIES)
