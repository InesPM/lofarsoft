
## ==============================================================================
##
##                                                Build instructions for ibraries
##
## ==============================================================================

## libsigproc

include_directories (
  ${CMAKE_INSTALL_PREFIX}/include
  ${PULSAR_BINARY_DIR}/apps/sigproc
  ${presto_SOURCE_DIR}/include
  )

set (sigproc_sources add_channels.c
add_samples.c
#alfa_position.c
aliases.c
angle_split.c
aoftm.c
bandfactors.c
#bandpass.c
#barycentre.c
bit.c
#blanker.c
#bpp.c
bpp2fb.c
#brute.c
cel2gal.c
#cfits_subs.c
chan_freqs.c
#clip.c
#clipper.c
close_log.c
#conv.c
#convertSigM.c
cprofc.c
#decimate.c
decimate_data.c
decimate_header.c
#dedisperse.c
dedisperse_data.c
dedisperse_header.c
deg2dms.c
#depolyco.c
#dice.c
dmdelay.c
dmshift.c
#downsample.c
eraseDP.c
error_message.c
#extract.c
#fake.c
#fbview.c
fetch_hdrval.c
ffreq.c
file_exists.c
#filterbank.c
filterbank_header.c
#flatten.c
#flux.c
#fold.c
fold_data.c
fold_header.c
fshift_prof.c
#getpulse.c
gmrt2fb.c
#gmrt2fil.c
#grayplot.c
#head_parse.c
#header.c
help.c
ignored_channels.c
indexx.c
inv_cerf.c
lookup.c
machine2prf.c
mjd.c
#mkheader.c
#newfind.c
norm_prof.c
np2.c
nrselect.c
nrsort.c
nrutil.c
nsamples.c
ooty2fb.c
open_file.c
open_files.c
open_log.c
pack_unpack.c
#pgplotter.c
phcalc.c
#polyco2period.c
print_version.c
process.c
prof_adds.c
#prof_cent.c
prof_ddis.c
prof_sbas.c
prof_sumc.c
prof_sumifs.c
#profile.c
pspm2fb.c
pspm_decode.c
pspm_prof.c
pspm_tstart.c
#psrfits.c
pulsar2k2fb.c
pulse.c
put.c
random.c
read_aoscan.c
read_block.c
read_header.c
read_polyco.c
#readchunk.c
#reader.c
readtim.c
recipes.c
scale_prof.c
scaledata.c
scamp2fb.c
select.c
send_stuff.c
shift_prof.c
single_ch.c
sizeof_file.c
slalib.c
smooth.c
#snrdm.c
#splice.c
#splitter.c
ssm.c
strings_equal.c
submean.c
submedian.c
swap_bytes.c
thresh_1d.c
#toi.c
#tree.c
#tune.c
typeof_inputdata.c
update_log.c
uttime.c
vanvleck.c
vmax.c
vmin.c
#wapp2fb.c
#wapp_header.c
wapp_prof.c
wappcorrect.c
whiten.c
write_epn.c
write_profiles.c
write_pulses.c
${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c
#${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c
)

set (sigproc_binaries
    filterbank fake bandpass reader decimate
    sigproc_fold polyco2period sigproc_profile sigproc_splice flux barycentre
    depolyco sigproc_downsample splitter snrdm blanker 
    header flatten dedisperse clip sigproc_tree extract clipper gmrt2fil
) 

set (sigproc_fc_binaries
   pgplotter tune getpulse
)

set (sigproc_scripts
  sigproc_quicklook
  foldsignals
  hunt
  ahunt
  csearch
  accn
  polyco
  makedmlist
  monitor
  makedummy
  )

##__________________________________________________________
## libsigproc

add_library (sigproc STATIC ${sigproc_sources})

target_link_libraries (sigproc
#  ${FFTW3_FFTW3F_LIBRARY}
#  ${FFTW3_LIBRARIES}
#  ${CFITSIO_LIBRARIES}
#  ${PGPLOT_LIBRARIES}
#  ${X11_LIBRARIES}
  )

if (UNIX)
 if (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
#     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
     COMPILE_FLAGS "-O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
     LINK_FLAGS "-static"
     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
     )
 else (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
#     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Dlinux"
     COMPILE_FLAGS "-O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Dlinux"
     LINK_FLAGS "-static"
     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
     )
 endif (APPLE)
endif (UNIX)

add_dependencies (sigproc  
  fftw
  sigproc_fort
  sigproc_quicklook
  foldsignals
  hunt
  csearch
  accn
  polyco
  makedmlist
  monitor
  makedummy
  )

##__________________________________________________________
## lib fortran

add_subdirectory (fortran)

## ==============================================================================
##
##  Executables
##
## ==============================================================================

##__________________________________________________________
## Build instructions for executables

add_executable (filterbank
#  filterbank.c head_parse.c ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c wapp2fb.c
  filterbank.c head_parse.c wapp2fb.c
  )

add_executable (extract
  extract.c
  )

add_executable (fake
  fake.c
  )

add_executable (header
  header.c head_parse.c
  )

add_executable (bandpass
  bandpass.c
  )

add_executable (reader
  reader.c
  )

add_executable (decimate
  decimate.c
  )

add_executable (dedisperse
  dedisperse.c head_parse.c  
  )

add_executable (sigproc_fold
  fold.c
  )

add_executable (polyco2period
  polyco2period.c
  )

add_executable (sigproc_profile
  profile.c
  )

add_executable (flux
  flux.c
  )

add_executable (sigproc_splice
  splice.c
  )

add_executable (barycentre
  barycentre.c
  )

add_executable (depolyco
  depolyco.c
  )

add_executable (flatten
  flatten.c
  )

add_executable (clip
  clip.c
  )

add_executable (clipper
  clipper.c
  )

add_executable (sigproc_downsample
  downsample.c
  )

add_executable (splitter
  splitter.c
  )

add_executable (snrdm
  snrdm.c
  )

add_executable (blanker
  blanker.c
  )

add_executable (sigproc_tree
  tree.c
  )

add_executable (pgplotter
  pgplotter.c
  )

add_executable (getpulse
  getpulse.c
  )

add_executable (tune
  tune.c
  )

add_executable (gmrt2fil
  gmrt2fil.c
  )

## linker instructions

foreach (src ${sigproc_binaries})
  if (UNIX)
    if (APPLE)
      ## Additional target properties (Apple)
      set_target_properties (${src}
	PROPERTIES
	#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
	COMPILE_FLAGS "-O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
	CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
	CMAKE_SHARED_LINKER_FLAGS ""
	)
    else (APPLE)
      ## Additional target properties (non-Apple)
      set_target_properties (${src}
	PROPERTIES
	#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  -Dlinux"
	COMPILE_FLAGS "-O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  -Dlinux"
	CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
	CMAKE_SHARED_LINKER_FLAGS ""
	)
    endif (APPLE)
  endif (UNIX)
  ## Linker instructions
  target_link_libraries (${src}
    sigproc
    sigproc_fort
    ${FFTW3_FFTW3F_LIBRARY}
    ${FFTW3_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    ${libm}
    )
endforeach (src)


foreach (src ${sigproc_fc_binaries})
  set_target_properties (${src}
    PROPERTIES
    LINKER_LANGUAGE Fortran
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
    COMPILE_FLAGS "-O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src}
    sigproc
    sigproc_fort
    ${FFTW3_FFTW3F_LIBRARY}
    ${FFTW3_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    ${libm}
    ${PGPLOT_CPGPLOT_LIBRARY}
    )
endforeach (src)

#set (CMAKE_Fortran_COMPILER g77)
set (CMAKE_Fortran_COMPILER gfortran)


IF(NOT CMAKE_Fortran_LINK_EXECUTABLE)
  SET(CMAKE_Fortran_LINK_EXECUTABLE
    "<CMAKE_Fortran_COMPILER> <CMAKE_Fortran_LINK_FLAGS> <LINK_FLAGS> <FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
ENDIF(NOT CMAKE_Fortran_LINK_EXECUTABLE)

if (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  message (STATUS "[PULSAR] Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts exists!")
else (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  message (STATUS "[PULSAR] Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts does not exist!")
  message (STATUS "[PULSAR] Creating Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  EXECUTE_PROCESS( COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/sigproc/scripts)
endif (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")

add_custom_target (sigproc_quicklook
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/sigproc_quicklook
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND cat quicklook.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND cp  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/sigproc_quicklook
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook"
 )

add_custom_target (foldsignals
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
#  COMMAND echo "set bin = \\(${CMAKE_INSTALL_PREFIX}\\)" >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND cat foldsignals.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/foldsignals"
 )

add_custom_target (hunt
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND cat hunt.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/hunt
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/hunt"
 )

add_custom_target (ahunt
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/ahunt
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/ahunt
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/ahunt
  COMMAND cat ahunt.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/ahunt
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/ahunt
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/ahunt"
 )

add_custom_target (csearch
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND cat csearch.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/csearch
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/csearch"
 )

add_custom_target (accn
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND cat accn.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/accn
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/accn"
 )

add_custom_target (polyco
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/polyco ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/polyco
  COMMAND echo "\\#!"`which tclsh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND cat polyco.tcl >>  ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND cp ${CMAKE_INSTALL_PREFIX}/bin/polyco ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/polyco
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/polyco"
 )

add_custom_target (makedmlist
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND echo "\\#!"`which tclsh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND cat makedmlist.tcl >>  ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/makedmlist"
 )

add_custom_target (monitor
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/monitor
  COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/mkmonitor.csh ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND chmod +x ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/mkmonitor.csh
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/mkmonitor.csh > ${CMAKE_INSTALL_PREFIX}/bin/monitor
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/monitor
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/monitor"
 )

add_custom_target (makedummy
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  COMMAND cat ${PULSAR_SOURCE_DIR}/apps/sigproc/makedummy.csh > ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/makedummy"
 )
 
find_package(LATEX)

add_custom_target (sigproc_helpdocs
#  ADD_DEPENDENCIES ( sigproc_helpdocs
#  sigproc
#  filterbank
#  header
#  bandpass
#  reader decimate
#  splice
#  dedisperse
#  sigproc_fold
#  sigproc_profile
#  flux
#  barycentre
#  sigproc_quicklook
#  foldsignals
#  hunt
#  csearch
#  accn
#  polyco
#  makedmlist
#  monitor
#  makedummy
#  )
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/help.csh ${PULSAR_BINARY_DIR}/apps/sigproc/help.csh
  COMMAND ./help.csh filterbank > /dev/null
  COMMAND ./help.csh fake > /dev/null
  COMMAND ./help.csh header > /dev/null
  COMMAND ./help.csh bandpass > /dev/null
  COMMAND ./help.csh reader > /dev/null
  COMMAND ./help.csh decimate > /dev/null
  COMMAND ./help.csh splice > /dev/null
  COMMAND ./help.csh dedisperse > /dev/null
  COMMAND ./help.csh sigproc_fold > /dev/null
  COMMAND ./help.csh sigproc_profile > /dev/null
  COMMAND ./help.csh sigproc_splice > /dev/null
  COMMAND ./help.csh flux > /dev/null
  COMMAND ./help.csh scripts/polyco > /dev/null
  COMMAND ./help.csh barycentre > /dev/null
  COMMAND ./help.csh scripts/sigproc_quicklook > /dev/null
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/sigproc.tex ${PULSAR_BINARY_DIR}/apps/sigproc/sigproc.tex
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/version.history ${PULSAR_BINARY_DIR}/apps/sigproc/version.history
#  COMMAND ${LATEX_COMPILER} sigproc
  COMMAND ${PDFLATEX_COMPILER} sigproc
  COMMAND cp sigproc.pdf ${CMAKE_INSTALL_PREFIX}/doc/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/sigproc
  COMMENT "Creating help files for sigproc, including ${CMAKE_INSTALL_PREFIX}/doc/sigproc.pdf"
 )

find_package(BISON)

if (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  message (STATUS "[PULSAR] Yacc results file ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c exists!")
else (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  message (STATUS "[PULSAR] Creating yacc results ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  EXECUTE_PROCESS( COMMAND ${BISON_EXECUTABLE} -vdt -o ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c ${PULSAR_SOURCE_DIR}/apps/sigproc/mkheader.y)
endif (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")

find_package(FLEX)

if (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  message (STATUS "[PULSAR] Yacc results file ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c exists!")
else (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  message (STATUS "[PULSAR] Creating lex results ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  EXECUTE_PROCESS( COMMAND ${FLEX_EXECUTABLE} -o ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c ${PULSAR_SOURCE_DIR}/apps/sigproc/mkheader.l)
endif (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")


## ==============================================================================
##
##  Installation
## 
## ==============================================================================

install (
  TARGETS sigproc
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install (
  TARGETS ${sigproc_binaries} ${sigproc_fc_binaries} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Test programs
## 
## ==============================================================================

##add_executable (testbinresp
##  ${sigproc_SOURCE_DIR}/tests/testbinresp.c
##  ${PLOT2DOBJS}
##  )
