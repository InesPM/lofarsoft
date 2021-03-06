#! /usr/bin/env python

import os
import os.path
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import pycrtools as cr
from pycrtools import IO
import pytmf
from pycrtools.imager import CoordinateGrid

# Data directories
lofardata = os.environ["LOFARDATA"].rstrip('/')+'/'

# Filenames
filenames = [lofardata + "Lightning/" + n for n in ["lightning_17_23.h5"]]

print "Using files:"
for n in filenames:
    print n

# Parameters
startblock=225950
nblocks=1
blocksize=1024
station='CS302'
antennaset='LBA_INNER'
selection=None#range(0,48,2)
nfstart=0
nfend=513
nfreq=nfend-nfstart

# Open datafile object
crfile=IO.open(filenames, blocksize, selection)

# Set antennaset
crfile.setAntennaset(antennaset)

# Get calibration data and settings from file
nyquistZone=crfile["nyquistZone"]
antpos=crfile["RelativeAntennaPositions"]
nantennas=int(antpos.shape()[0])

# Set image parameters
imparam = {
    'NAXIS' : 2,
    'NAXIS1' : 90,
    'NAXIS2' : 90,
    'CTYPE1' : 'ALON-STG',
    'CTYPE2' : 'ALAT-STG',
    'LONPOLE' : 0.,
    'LATPOLE' : 90.,
    'CRVAL1' : 180.,
    'CRVAL2' : 90.,
    'CRPIX1' : 45.5,
    'CRPIX2' : 45.5,
    'CDELT1' : 2.566666603088E+00,
    'CDELT2' : 2.566666603088E+00,
    'CUNIT1' : 'deg',
    'CUNIT2' : 'deg',
    'PC001001' : 1.000000000000E+00,
    'PC002001' : 0.000000000000E+00,
    'PC001002' : 0.000000000000E+00,
    'PC002002' : 1.000000000000E+00
}

npix=imparam['NAXIS1']*imparam['NAXIS2']

# Generate coordinate grid
print "Generating grid"
grid=CoordinateGrid(**imparam)
print "Grid generation finished"

# Get frequencies
frequencies=crfile["Frequency"][range(nfstart,nfend)]
nfreq=len(frequencies)

# Initialize empty arrays
fxdata=crfile["emptyFx"]
fftdata=crfile["emptyFFT"]

image=cr.hArray(complex, dimensions=(imparam['NAXIS1'],imparam['NAXIS2'],nfreq), fill=0.)

image=cr.hArray(complex, dimensions=(imparam['NAXIS1'],imparam['NAXIS2'],nfreq), fill=0.)

for block in range(startblock, startblock+nblocks):

    print "processing block:", block

    crfile.readdata(fxdata, block)

    fxdata[...].applyhanningfilter()

    fftdata[...].fftcasa(fxdata[...], nyquistZone)

    # Beamform image
    print "Beamformer input:"
    print image.shape(), len(image)
    print fftdata.shape(), len(fftdata)
    print frequencies.shape(), len(frequencies)
    print antpos.shape(), len(antpos)
    print grid.cartesian.shape(), len(grid.cartesian)

    cr.hBeamformImage(image, fftdata, frequencies, antpos, grid.cartesian)

print "Image complete, storing to disk."

# Get image as numpy.ndarray
npimage = image.toNumpy()
npimage = np.square(np.abs(npimage)).sum(axis=2)

# Save image as FITS
hdu = pyfits.PrimaryHDU(npimage)

# Write WCS parameters to header
hdr = hdu.header

# Workaround for bug in casaviewer
imparam['CTYPE1']='??LN-STG'
imparam['CTYPE2']='??LT-STG'
keys = imparam.keys()
keys.sort()
for key in keys:
    hdr.update(key, imparam[key])

# Check if file exists and overwrite if so
if os.path.isfile('output.fits'):
    os.remove('output.fits')
hdu.writeto('output.fits')
