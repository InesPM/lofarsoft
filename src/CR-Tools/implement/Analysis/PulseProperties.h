/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schroeder (<mail>)                                              *
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

#ifndef PULSEPROPERTIES_H
#define PULSEPROPERTIES_H

// Standard library header files
#include <iostream>
#include <string>

// Library for ROOT Dictionary
#include <Rtypes.h>


//namespace CR { // Namespace CR -- begin
  
  /*!
    \class PulseProperties
    
    \ingroup Analysis
    
    \brief Contains basic pulse parameters like times and heights
    
    \author Frank Schroeder

    \date 2008/10/21

    \test tPulseProperties.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Don't forget to run the following commands in implement/Analysis,
      if something has changed:
      \verbatim
      rm RootDict.*
      rootcint RootDict.C -c PulseProperties.h
      \endverbatim
      <li>Namespace declaration causes problems with root dictionary
      should be solved in ROOT 5.20
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>

  */  
  class PulseProperties {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    PulseProperties ();
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~PulseProperties ();
    
    // ---------------------------------------------------------------- Variables
    //! height of pulse (usually after correction for noise)
    double height;
    //! error of pulse height (usually calculated from noise)    
    double heightError;
    //! time of pulse (usually time of envelope maximum)
    double time;
    //! error of pulse time (usually calculated from noise)    
    double timeError;
    //! ID of antenna which recorde the pulse
    int antennaID;
    //! number of antenna
    int antenna;
    //! maximum of trace
    double maximum;
    //! maximum of envelope of trace
    double envelopeMaximum;
    //! minimum
    double minimum;
    //! time of maximum
    double maximumTime;
    //! time of maximum of envelope
    double envelopeTime;
    //! time of minimum
    double minimumTime;
    //! time when crossing half height
    double halfheightTime;
    //! time shift by the beam forming
    double geomDelay;
    //! FWHM
    double fwhm;
    //! antenna position in shower coordinates x
    double distX;
    //! error of antenna position in shower coordinates x
    double distXerr;
    //! antenna position in shower coordinates y
    double distY;
    //! error of antenna position in shower coordinates y
    double distYerr;
    //! antenna position in shower coordinates z
    double distZ;
    //! error of antenna position in shower coordinates z
    double distZerr;
    //! distance of antenna position from shower axis sqrt(x^2+y^2)
    double dist;
    //! error of distance of antenna position from shower axis (gaussian error propagation)
    double disterr;
    //! angle between antenna and core position
    double angleToCore;
    //! error of angle between antenna and core position
    double angleToCoreerr;
    //! mean noise of a certain part of the trace (for lateral distribution analysis)
    double noise;
    //! calculated pulse height for exponential lateral distribution
    double lateralExpHeight;
    //! error of calculated pulse height for exponential lateral distribution
    double lateralExpHeightErr;
    //! relative deviation of calculated vs. measured pulse height
    double lateralExpDeviation;
    //! error of relative deviation of calculated vs. measured pulse height
    double lateralExpDeviationErr;
    //! will be set to true, if antenna did not pass the lateral distribution cuts
    bool lateralCut;
    //! polarization
    std::string polarization;
    //!sign of the signal (example positive if max-min >0)
    int minMaxSign;
    //!sign of the signal at the time of the maximum of the envelope
    int envSign;


    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the class

      \return className -- The name of the class, PulseProperties.
    */
    inline std::string className () const {
      return "PulseProperties";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);

    // ------------------------------------------------------------------ Methods


    //! Make class available in ROOT dictionary (needed for ROOT I/O)
    ClassDef(PulseProperties,1)
  };
  
//} // Namespace CR -- end

#endif /* PULSEPROPERTIES_H */
  
