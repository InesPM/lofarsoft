""" 
  Module beamformer.py. Part of the LOFAR Cosmic Ray pipeline.
  Beamformer class for the LOFAR pipeline. May be superseded by dedicated (wrapped) C++ functions.
  hBeamformImage is already there for tied-array (adding) beam.
        
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

from pycrtools import *

class Beamformer(object):
    """A beamformer class. As the name says, it does beamforming.
       Work in progress...
    """

    def __init__(self, crfile, cr_fft):
        nofAntennas = crfile["nofAntennas"]
        
        self.delays=hArray(float,dimensions=[nofAntennas])
        self.weights=hArray(complex,dimensions = cr_fft,name="Complex Weights")
        self.freqs = hArray(crfile["frequencyValues"]) # a FloatVec comes out, so put it into hArray
        self.phases=hArray(float,dimensions=cr_fft,name="Phases",xvalues=crfile["frequencyValues"]) 
        self.shifted_fft=hArray(complex,dimensions=cr_fft)
        
        self.beamformed_fft=hArray(complex,dimensions=[crfile["fftLength"]])
        
        self.tiedArrayBeam = hArray(float, dimensions=[crfile["blocksize"]])
        self.incoherentBeam = hArray(float, dimensions=[crfile["blocksize"]])
        self.ccBeam = hArray(float, dimensions=[crfile["blocksize"]])
        
        self.beamformed_smoothed=hArray(float,dimensions=[crfile["blocksize"]])

        self.azel=hArray(float,dimensions=[3])
        self.cartesian=azel.new()
  
  #antennaPositionsForIndices = 
  # FIX: cut out the right antennaIndices here! In antenna_positions, dimensions for new arrays, then beamforming...

    def tiedArrayBeam(azel_in, cr_fft, antennaPositions, antennaIndices):
        print 'Evaluating for az = %f, el = %f' % (azel_in[0], azel_in[1]),
        if ( azel_in[0] > 360. or azel_in[0] < 0. or azel_in[1] > 90. or azel_in[1] < 0.):
            erg = 0.
        else:
            self.azel[0] = azel_in[0]
            self.azel[1] = azel_in[1]
            if (FarField):
                self.azel[2] = 1.
            else:
                self.azel[2] = azel_in[2]
        hCoordinateConvert(self.azel, CoordinateTypes.AzElRadius, self.cartesian, CoordinateTypes.Cartesian, True)
        hGeometricDelays(self.delays, antennaPositions, self.cartesian, True)   
        hDelayToPhase(self.phases, self.freqs, self.delays) 
        hPhaseToComplex(self.weights, self.phases)

        hMul(self.shifted_fft, cr_fft, self.weights) # Dimensions don't match: need [...] ???
        
        self.beamformed_fft.fill(0.0)   
            
        self.shifted_fft[antennaIndices, ...].addto(self.beamformed_fft)

        hInvFFTw(self.tiedArrayBeam, self.beamformed_fft)
          
        return self.tiedArrayBeam
    
    
    def pulseMaximizer(azel_in, cr_fft, antennaPositions, antennaIndices):
        tiedArrayBeam = tiedArrayBeam(azel_in, cr_fft, antennaPositions, antennaIndices)
        tiedArrayBeam.abs() # make absolute value!
#        hRunningAverage(beamformed_smoothed, tiedArrayBeam, 5, hWEIGHTS.GAUSSIAN)

        value = - tiedArrayBeam.max()[0] / blocksize # just the maximum. 
        print ' value = %f ' % value
        return value
        
            
                    
  #      efield = beamformed_efield.toNumpy()
  #      plt.clf()
  #      plt.plot(efield.T / blocksize)
  #      plt.show()
#        beamformed_efield.abs() # make absolute value!
#        hRunningAverage(beamformed_efield_smoothed, beamformed_efield, 5, hWEIGHTS.GAUSSIAN)
        # make smoother by convoluting with a 5-tap gaussian window

  #       smoothedstuff = beamformed_efield_smoothed.toNumpy()
  #      plt.clf()
  #      plt.plot(smoothedstuff.T / blocksize)
  #      plt.show()

#        erg = - beamformed_efield.max()[0] / blocksize # just the maximum of beamformed_efield_smoothed !!!!
#        print ' value = %f ' % erg
        
        
         
        """Initializes the object.

        Required arguments:
        
        ========= =================
        Parameter Description
        ========= =================
        *a*       does nothing
        *b*       also does nothing
        ========= =================

        Optional arguments:
        
        ========= =======================
        Parameter Description
        ========= =======================
        *c*       [True (default)| False]
        ========= =======================

        """



# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

  #     test code for indexing arrays
  #      shifted_fft[range(0, nofAntennas, 2), ...].addto(beamformed_fft_even)
  #      shifted_fft[range(1, nofAntennas, 2), ...].addto(beamformed_fft_odd)
        
  #      beamformed_fft_odd.addto(beamformed_fft_test)
  #      beamformed_fft_even.addto(beamformed_fft_test)
        
  #      beamformed_fft_test2.subadd(beamformed_fft_test, beamformed_fft)
  #      beamformed_fft_test2.pprint()
