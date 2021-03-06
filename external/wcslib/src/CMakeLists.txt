
## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

set (WCSLIB_VERSION_MAJOR 4)
set (WCSLIB_VERSION_MINOR 2)
set (WCSLIB_VERSION "${WCSLIB_VERSION_MAJOR}.${WCSLIB_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## User defined build/install options

option (WCSLIB_VERBOSE_CONFIGURE "Verbose output during configuration?" NO  )
option (WCSLIB_FORCE_BUILD       "Force rebuild and local installation" NO  )
option (WCSLIB_ENABLE_TESTING    "Enable building and running tests?"   YES )
option (WCSLIB_PYTHON_WRAPPER    "Generate Python wrapper for WCSLIB?"  YES )

## Handle option: Verbose output during configuration?

if (NOT WCSLIB_VERBOSE_CONFIGURE)
  set (WCSLIB_FIND_QUIETLY  YES )
  set (CFITSIO_FIND_QUIETLY YES )
  set (BOOST_FIND_QUIETLY   YES )
  set (PYTHON_FIND_QUIETLY  YES )
endif (NOT WCSLIB_VERBOSE_CONFIGURE)

##__________________________________________________________
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

if (LUS_SOURCE_DIR)
  ## Load custom CMake settings
  include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
  ## Check if package is not installed yet
  if (NOT WCSLIB_FOUND)
    find_package (WCSLIB)
  endif (NOT WCSLIB_FOUND)
endif (LUS_SOURCE_DIR)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
  
  ## silence find scipt output if necessary
  
  if (NOT CFITSIO_FOUND)
    find_package (CFITSIO)
  endif (NOT CFITSIO_FOUND)
  
  find_package (Boost)
  find_package (Python)
  
  ## ----------------------------------------------------------------------------
  ## Check for additional tools which are required
  
  if (WCSLIB_ENABLE_TESTING)
    include (CTest)
    enable_testing()
  endif (WCSLIB_ENABLE_TESTING)
  
  if (NOT FLEX_FOUND)
    find_package (FLEX)
  endif (NOT FLEX_FOUND)
  
  ## ----------------------------------------------------------------------------
  ## Installation
  
  find_path (WCSLIB_INSTALL_PREFIX release_area.txt
    PATHS
    ${WCSLIB_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (WCSLIB_INSTALL_PREFIX)
    message (STATUS "Installation area located for package WCSLIB.")
    set (CMAKE_INSTALL_PREFIX ${WCSLIB_INSTALL_PREFIX}) 
  endif (WCSLIB_INSTALL_PREFIX)

  ## ----------------------------------------------------------------------------
  ## Subdirectories
  
  add_subdirectory (${WCSLIB_SOURCE_DIR}/C)
  add_subdirectory (${WCSLIB_SOURCE_DIR}/pgsbox)
#  add_subdirectory (${WCSLIB_SOURCE_DIR}/FORTRAN)
  
  if (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
    add_subdirectory (${WCSLIB_SOURCE_DIR}/Python)
  else (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
    message (STATUS "Unable to create Python bindings for WCSLIB!")
  endif (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
  
  ## ---------------------------------------------------------------------------
  ## Configuration summary
  
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
  message (STATUS "[WCSLIB] Configuration summary")
  message (STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE} ")
  message (STATUS "PYTHON_INCLUDES   = ${PYTHON_INCLUDES} ")
  message (STATUS "PYTHON_LIBRARIES  = ${PYTHON_LIBRARIES} ")
  message (STATUS "HAVE_PYCONFIG_H   = ${HAVE_PYCONFIG_H}")
  message (STATUS "WCSLIB_SOURCE_DIR = ${WCSLIB_SOURCE_DIR}")
  message (STATUS "WCSLIB_BINARY_DIR = ${WCSLIB_BINARY_DIR}")
  message (STATUS " ")
  message (STATUS "============================================================")
  message (STATUS " ")
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
  add_custom_target (install
    WORKING_DIRECTORY ${WCSLIB_BINARY_DIR}
    COMMENT "[WCSLIB] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
