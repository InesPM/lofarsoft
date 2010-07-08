#! /usr/bin/env python

from optparse import OptionParser
import numpy as np
import sys
import pyfits
import imager

## Parse commandline arguments
parser=OptionParser(usage="usage: %prog [options] output.fits", version="%prog 1.0")
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="verbose output")
(options, args)=parser.parse_args()

if not args:
    parser.print_help()
    sys.exit(0)

## Read in data
data = np.load('input_imager.npy')
antpos = np.load('CS302_antpos.npy')
frequencies = np.linspace(100, 200, data.shape[1])

## Set up imager
im = imager.Imager(frequencies)

## Process datablocks
datablock = np.zeros(1)

image = im.process(datablock)

## Save image as FITS
hdu = pyfits.PrimaryHDU(image)

try:
    hdu.writeto(args[0])
except IOError:
    print 'Error: could not write to', args[0]

