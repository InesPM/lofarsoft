
## Name of the project
project (MATHGL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATHGL_FORCE_BUILD       "Force rebuild and local installation?"  YES )
option (MATHGL_SVN_BUILD         "Build from the recent SVN version? "    NO  )
option (MATHGL_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (MATHGL_ENABLE_HDF5       "Enable support for HDF5?"               NO  )
option (MATHGL_ENABLE_PYTHON     "Enable support for Python?"             YES )
option (MATHGL_ENABLE_QT         "Enable support for Qt?"                 YES )
option (MATHGL_ENABLE_DOUBLE     "Enable double precision?"               YES )
option (MATHGL_DOWNLOAD_ARCHIVE  "Download archive of latest version?"    NO  )

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (MATHGL_FIND_QUIETLY YES)

find_package (MathGL)
find_package (Subversion)

find_program (find_executable find)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)

  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Tools and definitions
  
  list (INSERT CMAKE_PROGRAM_PATH 0 /sw/lib/qt4-mac/bin)
  list (INSERT CMAKE_PROGRAM_PATH 0 /sw/lib/qt4-x11/bin)
  
  find_program (ln_executable     ln    )
  find_program (qmake_executable  qmake )
  
  set (MATHGL_TAR_ARCHIVE "mathgl.tgz")
  set (MATHGL_URL "${USG_DOWNLOAD}/software/${MATHGL_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## External libraries
  
  if (NOT MATHGL_VERBOSE_CONFIGURE)
    set (GSL_FIND_QUIETLY    YES )
    set (HDF5_FIND_QUIETLY   YES )
    set (PYTHON_FIND_QUIETLY YES )
    set (QT_FIND_QUIETLY     YES )
  endif (NOT MATHGL_VERBOSE_CONFIGURE)
  
  set (DESIRED_QT_VERSION 4)
  set (QT4_FOUND NO)
  list (APPEND CMAKE_LIBRARY_PATH /sw/lib/qt4-mac)
  list (APPEND CMAKE_INCLUDE_PATH /sw/lib/qt4-mac/include)

  include (FindGSL)
  include (FindHDF5)
  include (FindPython)
  include (FindOpenGL)
  include (FindQt4)
  include (FindUnixCommands)

  ## Extra check for GSL _________________________

  if (NOT GSL_INCLUDES OR NOT GSL_LIBRARIES)
    set (HAVE_GSL FALSE)
  endif (NOT GSL_INCLUDES OR NOT GSL_LIBRARIES)

  ## Extra check for Qt4 _________________________
  
  if (NOT QT4_FOUND)
    include (FindQtFink)
    ## set variables for initial cache
    set (QT4_FOUND ${HAVE_QT})
    set (QT_INCLUDE_DIR ${QT_INCLUDES})
  endif (NOT QT4_FOUND)
  
  ## Python bindings _____________________________
  
  if (HAVE_PYTHON)
    
    find_path (NUMPY_INCLUDES numpy/arrayobject.h
      PATH_SUFFIXES
      python2.6/site-packages/numpy/core/include
      python2.5/site-packages/numpy/core/include
      python2.4/site-packages/numpy/core/include
      )
    
    if (NOT NUMPY_INCLUDES)
      message (STATUS "[MathGL] Python bindings disabled - no Numpy found!")
      set (MATHGL_ENABLE_PYTHON NO)
    endif (NOT NUMPY_INCLUDES)
    
  endif (HAVE_PYTHON)

  ##__________________________________________________________________
  ## Configure and build the project using CMake

  find_file (have_mathgl_tgz 
    mathgl-1.10.2.1.tgz
    mathgl-1.10.tgz
    mathgl.tgz
    PATHS ${MATHGL_SOURCE_DIR}
    )

  ## Create configuration files from templates

  configure_file (
    ${MATHGL_SOURCE_DIR}/mathgl.cmake.in
    ${MATHGL_BINARY_DIR}/tmp/mathgl.cmake
    )

  configure_file (
    ${MATHGL_SOURCE_DIR}/setup.py.in
    ${MATHGL_BINARY_DIR}/tmp/setup.py
    )

  ## Have CMake configure the project
  
  if (MATHGL_SVN_BUILD)
    ExternalProject_Add (mathgl
      PREFIX ${MATHGL_BINARY_DIR}
      DOWNLOAD_DIR ${MATHGL_BINARY_DIR}
      SOURCE_DIR ${MATHGL_BINARY_DIR}
      BINARY_DIR ${MATHGL_BINARY_DIR}/build
      SVN_REPOSITORY http://ufp.appl.sci-nnov.ru/svn/mathgl/trunk
      CMAKE_ARGS -C ${MATHGL_BINARY_DIR}/mathgl.cmake
      )
  else (MATHGL_SVN_BUILD)
    if (have_mathgl_tgz) 
      ExternalProject_Add (mathgl
	PREFIX ${MATHGL_BINARY_DIR}
	DOWNLOAD_DIR ${MATHGL_BINARY_DIR}
	SOURCE_DIR ${MATHGL_BINARY_DIR}
	BINARY_DIR ${MATHGL_BINARY_DIR}/build
	URL ${have_mathgl_tgz}
	URL_MD5 4122428797fd0c25a096c377b9ffe44a
	CMAKE_ARGS -C ${MATHGL_BINARY_DIR}/mathgl.cmake
	)
    else (have_mathgl_tgz)
      message (WARNING "[MthGL] Unable to build package - missing sources!")
    endif (have_mathgl_tgz)
  endif (MATHGL_SVN_BUILD)
  
  ## Pre-configure of configuration scripts
  ExternalProject_Add_Step (mathgl mathgl_config
    COMMAND mv ./tmp/setup.py .
    COMMAND mv ./tmp/mathgl.cmake .
    COMMENT "Setting up configuration scripts"
    DEPENDEES patch
    DEPENDERS configure
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    )
  
  ## Pre-configure patch of include paths
  ExternalProject_Add_Step (mathgl mathgl_links
    COMMAND rm -f gsl
    COMMAND ${ln_executable} -s ${GSL_INCLUDES}/gsl .
    COMMAND rm -f numpy
    COMMAND ${ln_executable} -s ${NUMPY_INCLUDES}/numpy .
    COMMENT "Creating symbolic links for include directories"
    DEPENDEES patch
    DEPENDERS configure
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/include
    )
  
  ## Pre-configure patch of include paths
  if (NOT MATHGL_SVN_BUILD)
    ExternalProject_Add_Step (mathgl mathgl_fonts
      COMMAND mkdir fonts
      COMMENT "Creating missing sub-directory."
      DEPENDERS install
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
      )
  endif (NOT MATHGL_SVN_BUILD)
  
  ##__________________________________________________________________
  ## Configuration summary
  
  if (MATHGL_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[MathGL] Summary of configuration status")
    message (STATUS "Forced rebuild from source  = ${MATHGL_FORCE_BUILD}")
    message (STATUS "Have GNU Scientific Library = ${HAVE_GSL}")
    message (STATUS "Have HDF5 Library           = ${HAVE_HDF5}")
    message (STATUS "Have NumPy                  = ${HAVE_NUMPY}")
    message (STATUS "Have Qt4                    = ${QT4_FOUND}")
    message (STATUS "Enable support for Python   = ${MATHGL_ENABLE_PYTHON}")
    message (STATUS "Enable support for Qt       = ${MATHGL_ENABLE_QT}")
    message ("-------------------------------------------------------------")
  endif (MATHGL_VERBOSE_CONFIGURE)
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  add_custom_target (mathgl
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    COMMENT "[MathGL] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Update the tar-archive to the latest version of the source

if (SUBVERSION_FOUND AND TAR_EXECUTABLE)
  add_custom_target (update_mathgl
    COMMAND rm -rf mathgl
    COMMAND ${Subversion_SVN_EXECUTABLE} co http://ufp.appl.sci-nnov.ru/svn/mathgl/trunk mathgl
    COMMAND ${TAR_EXECUTABLE} -cvzf mathgl.tgz mathgl --exclude .svn
    COMMAND rm -rf mathgl
    WORKING_DIRECTORY ${MATHGL_SOURCE_DIR}
    COMMENT "Retrieving latest version of MathGL source code ..."
    )
endif (SUBVERSION_FOUND AND TAR_EXECUTABLE)  
