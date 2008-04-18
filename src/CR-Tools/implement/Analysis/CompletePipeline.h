/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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

#ifndef COMPLETEPIPELINE_H
#define COMPLETEPIPELINE_H

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>

// This class is derived form CRinvFFT an adds upsampling
#include <Analysis/CRinvFFT.h>
// For Plots:
#include <Display/SimplePlot.h>
// For Upsampling:
#include <LopesStar/reconstruction.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CompletePipeline
    
    \ingroup Analysis
    
    \brief Brief description for class CompletePipeline
    
    \author Frank Schroeder

    \date 2008/01/30

    \test tCompletePipeline.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    This pipeline shall contain upsampling capabilities but is not working yet.
    
    <h3>Example(s)</h3>
    
  */  
  class CompletePipeline : public CRinvFFT {

  protected:
    //! Start time of the interval diplayed in the plot
    double plotStart_p;
    //! Stop time of the interval diplayed in the plot
    double plotStop_p;
    //! Time window for peak search of radio pulse
    double ccWindowWidth_p;
    //! File names of created plots 
    vector <string> plotlist;
    //! Last upsampling exponent (-1 if no upsampling was done so far)
    int lastUpsamplingExponent;
    //! Contains the upsampled fieldstrength
    Matrix<Double> upFieldStrength;
    //! Selection of upsampled antennas
    vector<bool> upsampledAntennas; 		// warning: don't use CasaCore-Vector as it makes only a flat copy using '='
    //! Last time upsampling exponent (-1 if no upsampling was done so far)
    int lastTimeUpsamplingExponent;
    //! Contains inerpolated time axis for upsampled fieldstrength
    Vector<Double> upTimeValues;


  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CompletePipeline ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CompletePipeline object from which to create this new
      one.
    */
    CompletePipeline (CompletePipeline const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CompletePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CompletePipeline object from which to make a copy.
    */
    CompletePipeline& operator= (CompletePipeline const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CompletePipeline.
    */
    std::string className () const {
      return "CompletePipeline";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    


    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the start time of the interval diplayed in the plot

      \return plotStart -- Start time of the interval diplayed in the plot
    */
    inline double getPlotStart () {
      return plotStart_p;
    }
    
    /*!
      \brief Set the start time of the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
    */
    inline void setPlotStart (double const &plotStart) {
      plotStart_p = plotStart;
    }
    
    /*!
      \brief Get the stop time of the interval diplayed in the plot

      \return plotStop -- Stop time of the interval diplayed in the plot
    */
    inline double getPlotStop () {
      return plotStop_p;
    }
    
    /*!
      \brief Set the stop time of the interval diplayed in the plot

      \param plotStop -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotStop (double const &plotStop) {
      plotStop_p = plotStop;
    }

    /*!
      \brief Set the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
      \param plotStop  -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotInterval (double const &plotStart,
				 double const &plotStop) {
      plotStart_p = plotStart;
      plotStop_p  = plotStop;
    }

    /*!
      \brief Get the list of the file names of all created plots

      \return plotlist -- list of created plots
    */
    inline vector<string> getPlotList () {
      return plotlist;
    }


    /*!
      \brief Gets the upsampled fieldstrength of selected antennas (data are stored for reuse)

      \param DataReader       -- DataReader (LopesEventIn)
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param antennaSelection -- Selection of antennas

      \return UpsampledFieldstrength -- Contains the upsampled time series of the selected antennas in colums of matrix
                                        (number of columns = number of elements in the antennaSelection, but columns
                                         corresponding to not selected antennas will not contain meaningless values.)
    */
    Matrix<Double> getUpsampledFieldstrength (DataReader *dr,
					      const int& upsampling_exp,
				 	      Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Gets the upsampled raw data FX of selected antennas (data are not stored for reuse)

      \param DataReader       -- DataReader (LopesEventIn)
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param antennaSelection -- Selection of antennas

      \return UpsampledFX     -- Contains the upsampled time series of the selected antennas in colums of matrix
                                 (number of columns = number of elements in the antennaSelection, but columns
                                 corresponding to not selected antennas will not contain meaningless values.)
    */
    Matrix<Double> getUpsampledFX (DataReader *dr,
				   const int& upsampling_exp,
		                   Vector<Bool> antennaSelection = Vector<Bool>(),
				   const bool& offsetSubstraction = false);

    /*!
      \brief Gets the time axis for upsampled data (data are stored for reuse)

      \param DataReader       -- DataReader (LopesEventIn)
      \param upsampling_exp   -- a value > 0 means that there are 2^upsampling_exp data points
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.


      \return UpsampledTime Axis -- The extended time axis containing new time values between the old ones.
                                    The first time value is the same as the first value of the original time axis.
                                    This means, that there are 2^upsampling_exp -1 data points attached after the
                                    last time value of the original axis.
    */
    Vector<Double> getUpsampledTimeAxis (DataReader *dr,
					 const int& upsampling_exp);


    /*!
      \brief Gets a Slice with the plot interval for the given xaxis

      \param xaxis            -- a time axis (may be upsampled or not)

      \return plotRange       -- A Slice containing the indices of the xaxis interval corresponding to the
                                 class variables plotStart_p and plotStop_p
    */
    Slice calculatePlotRange (const Vector<Double>& xaxis) const;



    /*!
      \brief Gets a Slice with the time interval BEFORE the plotrange 
             for the given xaxis

      \param xaxis            -- a time axis (may be upsampled or not)

      \return plotRangeNoise  -- A Slice containing the indices of the xaxis interval corresponding to the
                                 class variables plotStart_p and plotStop_p
    */
    Slice calculateNoiseRange (const Vector<Double>& xaxis) const;



    /*!
      \brief Gets a Slice with the time interval around the CC-beam center,
             the range is set via key ccWindowWidth

      \param xaxis             -- a time axis (may be upsampled or not)
      \param ccBeamcenter      -- position in time of the CC-beam center

      \return calculateCCRange -- A Slice containing the indices of the xaxis interval corresponding to the
                                  to a window around the CC beam center
    */
    Slice calculateCCRange (const Vector<Double>& xaxis, 
                            const double& ccBeamcenter) const;


    /*!
      \brief Plots the CC-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param fittedCCbeam     -- If supplied, the result of the fit will be plotted
      \param antennaSelection -- Selection of antennas considered for the plot
      \param ccBeamOffset     -- Constant noise (mean) of the cc-beam will be subtracted
      \param pBeamOffset      -- Constant noise (mean) of the power beam will be subtracted
    */

    void plotCCbeam (const string& filename, 
                     DataReader *dr,
                     Vector<Double> fittedCCbeam = Vector<Double>(),
                     Vector<Bool> antennaSelection = Vector<Bool>(),
 	 	     const double& ccBeamOffset = 0,
 	 	     const double& pBeamOffset = 0);

    /*!
      \brief Plots the X-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param fittedXbeam      -- If supplied, the result of the fit will be plotted
      \param antennaSelection -- Selection of antennas considered for the plot
      \param xBeamOffset      -- Constant noise (mean) of the x-beam will be subtracted
      \param pBeamOffset      -- Constant noise (mean) of the power beam will be subtracted
    */

    void plotXbeam (const string& filename, 
                    DataReader *dr,
                    Vector<Double> fittedXbeam = Vector<Double>(),
                    Vector<Bool> antennaSelection = Vector<Bool>(),
		    const double& xBeamOffset = 0,
		    const double& pBeamOffset = 0);

    /*!
      \brief Plots the fieldstrength of all antennas after beam forming

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
      \param seperated        -- true = an individual plot for each antenna will be created
                                 (the antenna number serves as file name appendix).
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param rawData          -- plot the raw ADC data instead of the calibrated fieldstrength
    */

    void plotAllAntennas (const string& filename,
                          DataReader *dr,
                          Vector<Bool> antennaSelection = Vector<Bool>(),
                          const bool& seperated = false,
                          const int& upsampling_exp = 0,
                          const bool& rawData = false);

    /*!
      \brief Prints the height of the maximum and the corresponding time in the plot range

      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param rawData          -- uses the raw ADC data instead of the calibrated fieldstrength
    */

    void calculateMaxima (DataReader *dr,
                          Vector<Bool> antennaSelection = Vector<Bool>(),
                          const int& upsampling_exp = 0,
                          const bool& rawData = false);
    /*!
      \brief same as calculateMaxima, but different output form

      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas
      \param upsampling_exp   -- a value > 0 means that data are upsampled by a factor of 2^upsampling_exp
                                 use e.g. uspampling_exp = 1 to have twice as many points plotted.
      \param cc_center        -- center of CC-beam fit
    */
    void listCalcMaxima (DataReader *dr,
                         Vector<Bool> antennaSelection = Vector<Bool>(),
                         const int& upsampling_exp = 0,
                         const double& cc_center = 0);
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (CompletePipeline const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* COMPLETEPIPELINE_H */
  
