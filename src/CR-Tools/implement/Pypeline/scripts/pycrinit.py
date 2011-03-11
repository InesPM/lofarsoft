#! /usr/bin/env python

from pycrtools import *
from getopt import *
hInit()

try:
    opts, args = getopt(sys.argv[1:],"hi:", ["help","include="])
except GetoptError:
    sys.exit(2)
for opt, arg in opts:
    # Display Help info
    if opt in ("-h", "--help"):
        help()
    # Execute yet another script if added in the parameter list
    elif opt in ("-i", "--include"):
        execfile(arg)
