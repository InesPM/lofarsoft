#!@SH_EXECUTABLE@
# $Id: plplot-test.sh.cmake 7435 2007-03-21 02:47:30Z airwin $
#
# Copyright (C) 2004, 2007  Alan W. Irwin
# Copyright (C) 2004  Rafael Laboissiere
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Library Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# test suite for executing all configured demos (either in plplot/tmp
# or else in the install area) capable of file output.  Our ultimate aim
# is to compare the generated files with results generated on a system
# that is known to give good results.  However, single-precision
# contamination (even when double is configured) and the consequent large
# roundoff error is currently hampering our cross-platform comparisons.
# So for now, this is only a test suite in the sense that it checks the
# files corresponding to our demo plots can all be generated.  It is
# up to the user to display those files with the appropriate software
# (e.g., gv for postscript files and kview or a browser for png, gif,
# or jpeg files on Linux systems) to make sure they display properly.

version=@VERSION@

EXAMPLES_DIR=${EXAMPLES_DIR:-.}
SRC_EXAMPLES_DIR=${SRC_EXAMPLES_DIR:-.}
OUTPUT_DIR=${OUTPUT_DIR:-.}
device=${DEVICE:-psc}
export EXAMPLES_DIR SRC_EXAMPLES_DIR OUTPUT_DIR device 

usage()
{
echo '
Usage: plplot-test.sh [OPTIONS]

Options:
   [--device=DEVICE] (DEVICE = one of psc [default], ps, psttfc, psttf, pstex,
                      png, gif, jpeg, cgm,svg, xfig, or plmeta)
   [--front-end=FE]  (FE = one of c, cxx, f77, f95 java, octave, python, tcl, perl, or ada)
                     If this option is not specified, then all front-ends will
                     be tested.  More than one front-end may be given, like
                     this --front-end="c cxx"
   [--examples-dir=/path/to/examples/dir]
                     Specify path to the examples directory.  Defaults to "."
   [--src-examples-dir=/path/to/src/examples/dir]
                     Specify path to the src examples directory.  Defaults to "."
   [--output-dir=/path/to/output/dir]
                     Specify location where the resulting files are stored.
                     Defaults to "."
   [--version]
   [--help]

Environment variables:
   DEVICE, FRONT_END, EXAMPLES_DIR, SRC_EXAMPLES_DIR, and OUTPUT_DIR can be 
   used to specify the devices, front-ends, the examples directory, the
   source examples directory (used for the special case of build-tree checks 
   when that tree is separated from the source tree), and output directory.
   These environment variables are overridden by the options --device,
   --front-end, --examples-dir, --src-examples-dir, and --output-dir.
'
   exit $1
}

while test $# -gt 0; do
   if test "@HAVE_BASH@" = ON ; then
      case "$1" in
      -*=*) optarg=${1#*=} ;;
      *) optarg= ;;
      esac
   else
      case "$1" in
      -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
      *) optarg= ;;
      esac
   fi

   case $1 in
      --device=*)
         device=$optarg
         ;;
      --version)
         echo $version
	 exit 0
         ;;
      --front-end=*)
         FRONT_END=$optarg
         for i in $FRONT_END ; do
	 test $i = "c"         \
              -o $i = "cxx"    \
              -o $i = "f77"    \
              -o $i = "f95"    \
              -o $i = "java"   \
              -o $i = "octave" \
              -o $i = "python" \
              -o $i = "tcl"    \
              -o $i = "perl"    \
              -o $i = "ada"    \
         || usage 0 1>&2
         done
	 ;;
      --examples-dir=*)
         EXAMPLES_DIR=$optarg
         ;;
      --src-examples-dir=*)
         SRC_EXAMPLES_DIR=$optarg
         ;;
      --output-dir=*)
         OUTPUT_DIR=$optarg
         ;;
      --help)
         usage 0 1>&2
         ;;
      *)
         usage 1 1>&2
         ;;
   esac
   shift
done

# This script is only designed to work when EXAMPLES_DIR is a directory
# with a subdirectory called "c".  Check whether this conditions is true.

if test ! -d $EXAMPLES_DIR/c ; then
echo '
This script is only designed to work when the EXAMPLES_DIR environment
variable (overridden by option --examples-dir) is a directory with a
subdirectory called "c".  This condition has been violated.
'
exit 1
fi

# These variables set by default assuming you are going to run this
# script from the installed demos directory $prefix/lib/plplot$version/examples.
cdir=$EXAMPLES_DIR/c
cxxdir=$EXAMPLES_DIR/c++
f77dir=$EXAMPLES_DIR/f77
f95dir=$EXAMPLES_DIR/f95
pythondir=$EXAMPLES_DIR/python
tcldir=$EXAMPLES_DIR/tcl
perldir=$SRC_EXAMPLES_DIR/perl
javadir=$EXAMPLES_DIR/java
adadir=$EXAMPLES_DIR/ada
octave=@OCTAVE@
octavedir=\
$EXAMPLES_DIR/../bindings/octave:\
$SRC_EXAMPLES_DIR/../bindings/octave/PLplot:\
$SRC_EXAMPLES_DIR/../bindings/octave/PLplot/support:\
$SRC_EXAMPLES_DIR/../bindings/octave/demos:\
$SRC_EXAMPLES_DIR/../bindings/octave/misc:\
$SRC_EXAMPLES_DIR/octave:\
@PLPLOT_OCTAVE_DIR@:\
@PLPLOT_OCTAVE_DIR@/support:\
@OCTAVE_M_DIR@/PLplot:\
@OCTAVE_OCT_DIR@:
PATH=$EXAMPLES_DIR/../utils:@exec_prefix@/bin:$PATH
export cdir cxxdir f77dir f95dir pythondir javadir octave octavedir tcldir perldir adadir PATH

fe=""

case "$device" in
   ps|psc)
      dsuffix=ps
      options=
      export dsuffix options
      ;;
   pstex)
      dsuffix=eps
      options=
      export dsuffix options
      ;;
   psttf|psttfc)
      dsuffix=psttf
      options=
      export dsuffix options
      ;;
   png)
      dsuffix=png
      options="-fam -fflen 2"
      export dsuffix options
      ;;
   gif)
      dsuffix=gif
      options="-fam -fflen 2"
      export dsuffix options
      ;;
   jpeg)
      dsuffix=jpeg
      options="-fam -fflen 2"
      export dsuffix options
      ;;
   svg)
      dsuffix=svg
      options=
      export dsuffix options
      ;;
   cgm)
      dsuffix=cgm
      options=
      export dsuffix options
      ;;
   xfig)
      dsuffix=fig
      options="-fam -fflen 2"
      export dsuffix options
      ;;
   plmeta)
      dsuffix=plmeta
      options=
      export dsuffix options
      ;;
   *)
      usage 1 1>&2
      ;;
esac

# Find out what front-ends have been configured
if test -z "$FRONT_END" ; then
   FRONT_END=c
   test "@ENABLE_cxx@" = ON && FRONT_END="$FRONT_END cxx"
   test "@ENABLE_f77@" = ON    && FRONT_END="$FRONT_END f77"
   test "@ENABLE_f95@" = ON    && FRONT_END="$FRONT_END f95"
   test "@ENABLE_java@" = ON   && FRONT_END="$FRONT_END java"
   test "@ENABLE_octave@" = ON && FRONT_END="$FRONT_END octave"
   test "@ENABLE_python@" = ON && FRONT_END="$FRONT_END python"
   test "@ENABLE_tcl@" = ON    && FRONT_END="$FRONT_END tcl"
   test "@ENABLE_pdl@" = ON    && FRONT_END="$FRONT_END perl"
   test "@ENABLE_ada@" = ON    && FRONT_END="$FRONT_END ada"
fi

# Find where the front-end scripts are by looking at the directory name of the
# current script.  

if test "@WIN32@" = "1"; then
   scripts_dir=${0%/*}
else
   scripts_dir=`echo $0 | sed 's:/[^/][^/]*$::'`
fi

# Call the front-end scripts
status=0

for i in $FRONT_END ; do
   echo "Testing front-end $i"
   script=$scripts_dir/test_$i.sh
   if test "@WIN32@" != "1"; then
      chmod +x $script
   fi
   @SH_EXECUTABLE@ $script || status=1
done

exit $status
