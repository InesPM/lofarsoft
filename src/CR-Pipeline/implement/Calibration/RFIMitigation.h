/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _RFIMITIGATION_H_
#define _RFIMITIGATION_H_

// CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <scimath/Functionals/Interpolate1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ArrayColumn.h>

// Custom header files
#include <Math/StatisticsFilter.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class RFIMitigation.h
    
    \ingroup Calibration
    
    \brief Brief description for class RFIMitigation
    
    \author Kalpana Singh
    
    \date 2006/03/15
    
    \test tRFIMitigation.cc
    
    <h3>Synopsis</h3>
    
    Extract a gaincurve from a complex spectra of \f$N_{\rm Ant}\f$ array elements.
    Also provides the functionality to normalize a complex gain spectra by the 
    extracted the gaincurves. Then normalized gain spectra is subjected for the
    rejection of noise signals by adopting a certain kind of filter. 
    
    The gain curve is extracted by the following method:
    
    Given a discrete complex spectrum
    \f[ f[\nu_{k}] = A[\nu_{k}] \exp(i \phi[\nu_{k}]) \f]
    where
    \f[ k=0, 1, 2...(N_{\rm FreqChan}-1) \f]
    and
    \f[ \nu_{\rm Bandwidth} \equiv \nu_{(N_{\rm FreqChan}-1)}-\nu_{0} \f]
    we extract the amplitudes of the terms,
    \f$A[\nu_{k}]\f$, and call these the terms of the absolute spectrum.
    The absolute spectrum is then divided into
    \f$N_{\rm Segments}\f$ segments, referred to as frequency sub bands. 
    Each sub band consists of \f$M_{\rm FreqChan}\f$ members, where
    \f[ M_{\rm FreqChan} = \frac{N_{\rm FreqChan}}{N_{\rm SubBands}} \f]
    According to the scanning method ( by impelementing median filter) one of
    these members is chosen as representative of the gain curve on that sub band.
    
    A natural cubic spline is used to interpolate the gain curve for any
    frequency sub bands across the original segment. 
    Also provides the functionality to normalize a complex gain spectra by
    the extracted the gaincurves. Then normalize gain spectra is subjected 
    for the rejection of noise signals. 
    
    <h3>Terminology</h3>
    
    In the RFIMitigation class, we employ specific terminology. To clarify
    subtle differences, the important terms (and their derivatives) are
    explained below in logical groups:
    <ul>
      <li><em>segmentationVector</em>
      <li><b>gainValues</b> --  Complex spectra of \f$N_{\rm Ant}\f$ array
      elements with rows equal to number of frequency channels =
      (dataBlockSize/2)+1 and columns define the number of antennas selected for
      processing. 
      <li><b> dataBlockSize </b> -- Has to be provided by the user, which will
      define number of frequency channels.
      <li><b> nOfSegments </b> -- no of segments in which array of gain curves
      has to be segmented, default value is 25.
    </ul>    
    
    <h3>Motivation</h3>
    
    This class was motivated by two needs:
    <ol>
      <li>We wish to to remove RFIs from gaincurves extracted from the antennas.
      To do this, we need some relative measure of gain.
      <li>We wish to flag regions of RFI. The method applied is to normalize a
      spectrum by dividing it by its underlying gain curve. From this normalized
      spectrum, we are able to easily identify RFI flooded regions. And then
      deviations from average gaincurves is measured, minimum noise levl has
      been calculated, and for the frequencyindices where RFI is identified that
      gaincurve values is substituted by the inverse of the normalized gaincurve
      value. 
    </ol>
  */
  
  class RFIMitigation {
    
  private:
    
    //! Number of the first channel in the segment
    uint segmentInit;
    //! Number of the last channel in the segment
    uint segmentFinal;
    
    // --- Construction ----------------------------------------------------------
    
    // Matrix<DComplex> array;
    // 
    // Vector<uint> segIndices;
    // 
    // uInt strength_filter ;
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor without argument
    */
    RFIMitigation ();
    
    /*!
      \brief Argumented Constructor

      \todo Function body still empty -- none of the provided parameters is
      processed!
      
      Sets the data array of the gain curves to the 2-dimensional specifications, 
      number of rows indicate the number of frequency indices, however no of columns 
      indicate the number of antennas to be scanned.
      
      \param spectra       -- two dimensional array of data that depends on number
                              of antennas and the whole frequency range to be scanned.
      \param nOfSegments   -- # of segments in which the gainvalue array has to be
                              divided.
      \param dataBlockSize -- Dimension of the array of gaincurves, number of
                              frequency channels are related like frequency channels
			      = (dataBlockSize/2)+1 .
    */
    RFIMitigation (const Matrix<DComplex>& spectra,
		   const uint& nOfSegments,
		   const uint& dataBlockSize );

    // --- Destruction ----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    virtual ~RFIMitigation ();
    
    
    // --- Computation methods --------------------------------------------------
    
    /*!
      \brief absolute gainvalues of the input complex (fft) data
      
      \param spectra -- two dimensional complex array of data that depends on 
                        number of antennas and the whole frequency range to be
			scanned.
      
      \return gainValues -- [??,??] The array of absolute values of gain
    */
    Matrix<Double> getAbsoluteGainValues(Matrix<DComplex> const &spectra);
    
    
    /*!
      \brief Frequency indices with respect to which segmentation of the gain curve array has to be done.
      
      \param dataBlockSize -- Dimension of the array of gaincurves.
      \param nofSegments   -- No of segments in which the gainvalue array has to
                              be divided.
      
      \return segments -- [nofSegments] the segmentation vector which consist of
                          frequency indices for segmentation.
    */
    Vector<uint> getSegmentationVector(uint const &dataBlockSize,
				       uint const &nofSegments);
    
    
    /*!
      \brief Interpolation of spectral trend
      
      Interpolation of spectral trend after extraction with one of the scanning
      method for requested antenna and frequency index.

      Scan the argument matrix with any statistical filter, the arguement must be
      of the same dimensions, i.e., no. of frequency bins and number of antennas
      must be same as defined at construction.

      Construct a linear interpolation across the argument 'gains' after
      extraction of gainvalues within each segment with median filter. The
      returned matrix will be of the same dimension of the input matrix.
      
      \param gainValues    -- Gain values across which to construct a linear
                              interpolation.
      \param dataBlockSize -- dimension of the array of gaincurves.
      \param nofSegments   -- # of segments in which the gainvalue array has to be
                              divided.
      
      \return interpolatedGains -- [??,??] the matrix of interpolated gain Values
                                   for each antenna requested.
    */
    Matrix<Double>  getinterpolatedGains (Matrix<DComplex> const &spectra,
					  uint const &dataBlockSize,
					  uint const &nofSegments );
    
    /*!
      \brief get difference of extracted gainValues with the interpolated gains.
      
      Give difference spectra of interpolated gainvalues with respect to the
      filtered spectra.
      
      \param gainValues    -- Gain Values across which to construct a linear
                              interpolation.
      \param dataBlockSize -- dimension of the array of gaincurves.
      \param nOfSegments   -- # of segments in which the gainvalue array has to
                              be divided.
      
      \return differenceSpectra -- [??,??] An array of difference spectra --
                                   difference of original spectra with interpolated
				   spectra.
    */
    Matrix<Double> getdifferenceSpectra (const Matrix<DComplex>& spectra,
					 const uint& dataBlockSize,
					 const uint& nOfSegments);
    
    /*!
      \brief Get normalized extracted gainValues with respect to the interpolated gains.
      
      Give normalized spectra of interpolated gainvalues with respect to the
      filtered spectra.
      
      \param gainValues	   -- gain Values across which to construct a linear
                              interpolation.
      \param dataBlockSize -- dimension of the array of gaincurves.
      \param nOfSegments   -- # of segments in which the gainvalue array has to
                              be divided.
      
      \return normalizedSpectra -- [??,??] an array of normalized spectra --
                                   normalization of original spectra with respect
				   to interpolated spectra.
    */
    Matrix<Double> getnormalizeSpectra( const Matrix<DComplex>& spectra,
					const uint& dataBlockSize,
					const uint& nOfSegments ) ;
    
    
    /*!
      \brief detect the position of RFI and remove it. 
      
      Position of RFI is detected by comparing the standard deviation value with the 
      minimum rms value for whole set of data of the normalized gainvalues array, and then at that position 
      gainvalue is replaced by inverse of the gainvalue.
      
      \param gainValues	--	gain Values across which to construct a spline.
      
      \param dataBlockSize  --	dimension of the array of gaincurves.
      
      \param nOfSegments    --      No of segments in which the gainvalue array has to be divided.
      
      \returns gainValue array with the same dimension as that of input spectra after removal of RFI.
      
    */
    
    
    Matrix<Double> getOptimizedSpectra( const Matrix<DComplex>& spectra,
					const uint& dataBlockSize,
					const uint& nOfSegments ) ;
    
  };
  
}  // Namespace CR -- end

#endif /* _RFIMITIGATION_H_ */
