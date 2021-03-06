#!/usr/bin/env ipython

"""Program for working with spectra from beam formed HDF5 files using DAL.

Features include:
* rebinning
* saving and reading from and to FITS
* plotting
* saving to PNG

Specview needs rebin.py and downsample.py both provided in the dynspec folder.

Frank Breitling, 2009

Update: Due to many changes on DAL and the BF format, this program became obsolete and serves for academic purposes.
The replacement tool is the spectrummaker.py"""


import pydal, pylab as pl, numpy as np, time, sys, warnings, pyfits, rebin, os

#if (len(sys.argv) < 2): sys.exit("Usage: spectrum.py path")

dt=3600     #time in seconds, 0 uses full range
imax=17    #imax=None #1e10 #limit intensity maximum, None is disabled
fmin=30; fmax=216; pixels=800;
ds_fits=1000;
filenames=['obs108_CS302LBA.h5','obs108_RS307HBA.h5','obs108_RS503HBA.h5']
#filenames=['obs108_CS302LBA.h5']
#os.listdir('.') #sys.argv[1]
    
pl.ion()
pl.rcParams['font.size']=15
pl.hold(False)
warnings.simplefilter("ignore",DeprecationWarning)
Fits_data=np.empty((0))

offset=0
for filename_h5 in filenames:         
    DDS=pydal.dalDataset(filename_h5,"HDF5")
    samples=DDS.openTable("beam000/SB000").getNumberOfRows()
    duration=samples*DDS.getAttribute_int("DOWNSAMPLE_RATE")/ \
              DDS.getAttribute_double("SAMPLE_RATE")
    if(dt):        ns=int(samples*dt/duration)
    else:          ns=samples
    ds=ns/pixels+1
    if ds*pixels>samples: ds=ds-1
    print "dt=",dt,", ds=",ds,", ns=",ns
    if (ds<ds_fits):
        ns=ds*pixels
    else:
        use_lowres=True
        filename_fits=filename_h5.replace('.h5','.fits')
        try:   Fits_data = pyfits.open(filename_fits)[0].data
        except IOError, err: os.system("python "+os.path.dirname(sys.argv[0])+
                                       "/downsample.py "+filename_h5)
        else:  print "reading "+filename_fits
        ds=int(ds/ds_fits)
        pixels=int(ns/ds/ds_fits)
        ns=ds*pixels
        samples=int(samples/ds_fits)

    dt=duration*ns/samples
    print "dt=",dt,", ds=",ds,", ns=",ns
    try:   Spectrum
    except NameError: Spectrum=np.zeros((fmax-fmin,pixels)) #Spectrum=np.empty((fmax-fmin,pixels))

    File = pydal.BeamFormed(filename_h5)
    Beam = File.getBeam(0)
    for s in range(Beam.nofSubbands()-2):
        tstart=time.time()
        if use_lowres:
            subband=rebin.rebin(Fits_data[s,0:ns],ds)
        else:
            subband=rebin.rebin(Beam.getIntensitySquared(s,0,ns),ds)
        Spectrum[offset+s,]=np.sqrt(subband)
        if (s%10==0 or s==Beam.nofSubbands()-3):
            print 'processing subband ',s,'(',time.time()-tstart,'s)'
            vmin=np.min(np.log(Spectrum.compress((Spectrum>1).flat)))
            sp=pl.imshow(np.log(Spectrum),extent=(0,dt,fmax,fmin),vmin=vmin,
                            aspect='auto', interpolation='nearest', vmax=imax)
            canvas=sp.figure.canvas
            try:   canvas.blit(sp.figure.bbox)
            except AttributeError: pl.draw(); canvas.blit(sp.figure.bbox)
    offset=offset+Beam.nofSubbands()

pl.grid()
cb=pl.colorbar(fraction=0.07, pad=0); cb.set_label("log10(intenisty)")
pl.title(filename_h5[0:6])
pl.xlabel('time [s]')
pl.ylabel('subband no.')  #pl.ylabel('frequency [MHz]')
#np.savez("preview.npz",Spectrum=Spectrum, dt=dt)
#Spectrum=np.load("Spectrum.npz")
pl.savefig(filename_h5[0:6]+".png")
print time.time()-tstart,"s"
pl.show()
