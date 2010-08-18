/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#ifndef SIMULATION2EVENT_H
#define SIMULATION2EVENT_H

// Standard library header files
#include <iostream>
#include <string>

#include <IO/TSmatrix2Event.h>
#include <IO/toLOPESgrid.h>
#include <IO/ApplyInstrumentEffects.h>
#include <IO/genTVSpec.h>
#include <IO/FalckeGorhamGalacticNoise.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class simulation2event
    
    \ingroup CR
    \ingroup IO
    
    \brief Generate a LOPES event file from reas simulation data
    
    \author Andreas Horneffer

    \date 2007/10/17

    \test tsimulation2event.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li> ImportSimulation 
      <li> Simulation2fftMatrix
      <li> ApplyInstrumentEffects
      <li> toLOPESgrid
      <li> genTVSpec
      <li> FalckeGorhamGalacticNoise
      <li> TSmatrix2Event
    </ul>
    
    <h3>Synopsis</h3>

    This class incorporates all the standard steps for the conversion of reas
    simulation data into a LOPES event file. It:
    <ol>
    <li> imports the simulation
    <li> applies the effects of the antenna (combines polarisation, applies frequency dependent gain)
    <li> applies the effects of the electronics (frequency dependent gain/phases, delays etc.)
    <li> interpolates the data onto the LOPES sampling grid
    <li> adds simulated TV-transmitter data (if requested)
    <li> adds simulated noise (if requested)
    <li> converts the data into a LOPES event file
    </ol>
    
    <h3>Example(s)</h3>
    
  */  
  class simulation2event {
    
    //! The CalTableReader
    CalTableReader *CTRead;

  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    simulation2event ();
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~simulation2event ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, simulation2event.
    */
    std::string className () const {
      return "simulation2event";
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
      \brief Set the CalTableReader object
      
      \param newCTRead - pointer to a new and initialized CalTableReader object
    */
    void setCalTable(CalTableReader *newCTRead){
      CTRead = newCTRead;
    }

    /*!
      \brief Open a simulation and generate a LOPES-style eventfile
      
      \param outfile -- name (including path) of the eventfile to be generated
      \param SimulationName -- name of the REAS parameter file (without extension
                               <tt>.reas</tt>)
      \param AntListName   -- name of the antenna list file (without extension
                            <tt>.list</tt>)
      \param path="."      -- directory in which the simulation data lies (without <tt>/</tt> at end)
      \param date=1104580800 -- "Observation" date of the event. Used to get the calibration data
                                and written into the event-file. (Default: 1. Jan. 2005 12:00 UTC)
      \param presync=32768 -- Number of samples before the "trigger" (default = LOPES default)
      \param AddTV=True    -- Add mockup-signals of the TV-transmitter
      \param AddNoise=True -- Add "Galactic" noise to the data
      
      \result returns true if read was successful
    */
    Bool generateEvent(String outfile,
		       String const SimulationName,
		       String const AntListName,
		       String const path=".",
		       uInt date=1104580800,
		       int presync=32768,
		       Bool AddTV=True,
		       Bool AddNoise=True); 
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* SIMULATION2EVENT_H */
  
