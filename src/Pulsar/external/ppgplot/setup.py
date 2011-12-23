#!/usr/bin/env python
from distutils.core import setup, Extension
from distutils.sysconfig import get_python_inc, get_python_lib
import os
import sys

name = "ppgplot"
version = "1.1"
description = "PGPLOT interface for Python originally made by Nick Patavalis"
author = "M.A. Breddels"
author_email = "breddels@astro.rug.nl"
url = "http://www.astro.rug.nl/~breddels/"

defines = []
includes = ["NUMERIC_INCLUDES-NOTFOUND", "NUMPY_INCLUDES-NOTFOUND", "NUMPY_INCLUDES-NOTFOUND/numpy", "/opt/local/include"]
extra_compile_args = []
libraries = ["cpgplot", "pgplot"]
library_dirs = ["/opt/local/lib", "/usr/local/lib"]

#for arg in sys.argv[2:]:
#    print arg
#    extra_compile_args.append(arg)
#    print extra_compile_args

    
includes.append(os.path.join(get_python_inc(plat_specific=1), "numpy"))
if os.name == "posix":
	libraries.append("X11")
	libraries.append("m")
	libraries.append("gfortran")
#	try:
#		library_dirs.append(os.environ["PGPLOT_DIR"])
#	except KeyError:
#		print >>sys.stderr, "WARNING: 'PGPLOT_DIR' env variable is not defined, might not find the libcpgplot.a library"

else:
	raise Exception, "os not supported"

#libraries.append(os.path.join(get_python_lib(plat_specific=1), "Numeric")+"/_numpy.so")

#print >>sys.stdout, "ppgplot SETUP.PY: includes - ", includes
#print >>sys.stdout, "ppgplot SETUP.PY: libraries - ", libraries
#print >>sys.stdout, "ppgplot SETUP.PY: library_dirs - ", library_dirs
#print >>sys.stdout, "ppgplot SETUP.PY: extra_compile_args - ", extra_compile_args

extension = Extension("ppgplot", ["ppgplot.c"],
	include_dirs=includes,
	libraries=libraries,
	library_dirs=library_dirs,
	define_macros=defines,
	extra_compile_args=extra_compile_args
	)



setup(name=name,
	version=version,
	description=description,
	author=author,
	author_email=author_email,
	url=url,
	ext_modules=[extension]
	)

