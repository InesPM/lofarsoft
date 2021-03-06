
## Name of the project
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Package information

set (ROOT_SVN "http://root.cern.ch/svn/root/trunk")

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ROOT_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (ROOT_CMAKE_BUILD       "Use CMake to configure and build ROOT?"  YES )
option (ROOT_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (ROOT_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (ROOT_FIND_QUIETLY YES)

if (NOT ROOT_FOUND)
  find_package (ROOT)
endif (NOT ROOT_FOUND)

## <-- begin build condition --------------------------------------------------->

if (NOT ROOT_FOUND OR ROOT_FORCE_BUILD)

  if (SVN_EXECUTABLE)
    
    if (UNIX)
      if (APPLE)
	set (root_architecture "macosx")
      else (APPLE)
	set (root_architecture "linux")
      endif (APPLE)
    else (UNIX)
      if (WIN32)
	set (root_architecture "win32gcc")
      endif (WIN32)
    endif (UNIX)
    
    ExternalProject_Add (root
      PREFIX ${ROOT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      SVN_REPOSITORY ${ROOT_SVN}
      CONFIGURE_COMMAND ${ROOT_BINARY_DIR}/root/configure ${root_architecture} --prefix=${CMAKE_INSTALL_PREFIX}
      )
    
  else (SVN_EXECUTABLE)
    
    add_custom_target (root
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}
      COMMENT "[ROOT] Unable to build package - missing Subversion to get code!"
      )
    
  endif (SVN_EXECUTABLE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT ROOT_FOUND OR ROOT_FORCE_BUILD)
  add_custom_target (root
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}
    COMMENT "[ROOT] Found system-wide installation; skipping rebuild!"
    )
endif (NOT ROOT_FOUND OR ROOT_FORCE_BUILD)
