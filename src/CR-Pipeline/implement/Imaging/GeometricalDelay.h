/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef GEOMETRICALDELAY_H
#define GEOMETRICALDELAY_H

#include <string>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

#ifdef HAVE_CASA
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#endif

#include <Imaging/CoordinateConversion.h>
#include <Math/VectorConversion.h>
#include <Math/VectorNorms.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class GeometricalDelay
    
    \ingroup Imaging
    
    \brief Geometrical delay for a set of antenna and source positions
    
    \author Lars B&auml;hren

    \date 2007/01/15

    \test tGeometricalDelay.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>S. Wijnholds (2006) LOFAR station calibration
    </ul>
    
    <h3>Synopsis</h3>

    <b>Geometry.</b>
    The basic equation, for which the delays are computed, is documented in
    the GeometricalWeight class. The procedure to arrive at the geometrical
    weights passed to the Beamform is divided into three steps, of which this
    class implements the first one.

    For a sky position \f$ \vec\rho \f$ and antenna positions \f$ \vec x_j \f$
    and \f$ \vec x_j \f$ the geometrical delay \f$ \tau_{ij} \f$ is given by
    \f[
      \tau_{ij} = \frac{1}{c} \left( |\vec \rho_j| - |\vec \rho_i| \right)
      = \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho - \vec x_i| \right)
    \f]

    Though being geometrically correct, the above expression is suitable for
    handling of baselines, rather than for the consideration of individual
    antennas (though their positions of course can be considered as a baseline
    w.r.t. to the array phase center). For an antenna located at \f$ \vec x_j \f$
    the source position \f$ \vec \rho \f$ appears at \f$ \vec \rho_j \f$ such
    the signals arrives with a delay of
    \f[ \tau_j = \frac{1}{c} \left( |\vec \rho_j| - |\vec \rho| \right) =
    \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho| \right) \f]

    <b>Array classes.</b>
    The class is constructed such, that both Blitz++ and CASA arrays are
    supported. However in order to minimize conversions, only one type is
    available at a time; this is obtained by encapsulating the array type 
    specific parts (including also private variables) in preprocessor statements:
    \code
    #ifdef HAVE_CASA
    // code working with the CASA array classes
    #else 
    #ifdef HAVE_BLITZ
    // code working with the Blitz++ array classes
    #endif
    #endif
    \endcode
    
    <h3>Example(s)</h3>
    
  */  
  class GeometricalDelay {
    
  protected:
    
#ifdef HAVE_CASA
    //! [nofAntennas,3] Antenna positions for which the delay is computed
    casa::Matrix<double> antPositions_p;
    //! [nofSkyPositions,3] Positions in the sky towards which to point
    casa::Matrix<double> skyPositions_p;
    //! [nofAntennas,nofSkyPositions] The geometrical delay itself
    casa::Matrix<double> delays_p;
#else
#ifdef HAVE_BLITZ
    //! [nofAntennas,3] Antenna positions for which the delay is computed
    blitz::Array<double,2> antPositions_p;
    //! [nofSkyPositions,3] Positions in the sky towards which to point
    blitz::Array<double,2> skyPositions_p;
    //! [nofAntennas,nofSkyPositions] The geometrical delay itself
    blitz::Array<double,2> delays_p;
#endif
#endif

    //! Buffer the values for the geometrical delay?
    bool bufferDelays_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      Geometrical delay for a single antenna position at the
      position of the phase center, \f$ \vec x = (0,0,0) \f$ and a single 
      source position at unit distance above the antenna, \f$ \vec \rho =
      (0,0,1) \f$
    */
    GeometricalDelay ();
    
    /*!
      \brief Argumented constructor (using Blitz arrays)

      If an error is encountered processing the input parameters, we revert to
      the individual default settings used with the default constructor; at least
      this way we end up with an operational object.

      \param antPositions -- [nofAntennas,3] Antenna positions for which the
                             delay is computed, \f$ (x,y,z) \f$
      \param skyPositions -- [nofSkyPositions,3] Positions in the sky towards
                             which to point, given in the same reference frame
			     as the antenna positions, \f$ (x,y,z) \f$
      \param bufferDelay  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    GeometricalDelay (casa::Matrix<double> const &antPositions,
		      casa::Matrix<double> const &skyPositions,
		      bool const &bufferDelay=false);
#else 
#ifdef HAVE_BLITZ
    GeometricalDelay (blitz::Array<double,2> const &antPositions,
		      blitz::Array<double,2> const &skyPositions,
		      bool const &bufferDelay=false);
#endif
#endif

    /*!
      \brief Copy constructor
      
      \param other -- Another GeometricalDelay object from which to create this
                      new one.
    */
    GeometricalDelay (GeometricalDelay const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~GeometricalDelay ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeometricalDelay object from which to make a copy.
    */
    GeometricalDelay& operator= (GeometricalDelay const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are the values for the geometrical delay buffered?

      \return bufferDelays -- Are the values for the geometrical delay buffered?
                              If set <i>yes</i> the delays will be computed from
			      the provided antenna and sky positions and afterwards
			      kept in memory; if set <i>no</i> only the input 
			      parameters are stored an no further action is taken.
     */
    inline bool bufferDelays () {
      return bufferDelays_p;
    }
    
    /*!
      \brief Enable/disable buffering of the values for the geometrical delay

      \param bufferDelays -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
    */
    inline void bufferDelays (bool const &bufferDelays=false) {
      bufferDelays_p = bufferDelays;
      if (bufferDelays) {
	setDelays();
      }
    }
    
    /*!
      \brief Get the antenna positions, for which the delay is computed

      \return antPositions -- Antenna positions for which the delays are computed,
                              [nofAntennas,3]
    */
#ifdef HAVE_CASA
    inline casa::Matrix<double> antPositions () {
      return antPositions_p;
    }
#else
#ifdef HAVE_BLITZ
    inline blitz::Array<double,2> antPositions () {
      return antPositions_p;
    }
#endif
#endif

    /*!
      \brief Set the antenna positions, for which the delay is computed
      
      \param antPositions -- The antenna positions, for which the delay is
                             computed, [nofAntennas,3]
      \param bufferDelays -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setAntPositions (const casa::Matrix<double> &antPositions,
			  const bool &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setAntPositions (const blitz::Array<double,2> &antPositions,
			  const bool &bufferDelays=false);
#endif
#endif

    /*!
      \brief Set a specific antenna position

      \param antPosition -- The 3-dimensional position of the antenna, [3].
      \param number      -- The number of the antenna, for which this new 
                            position information is provided; \f$ 0 \leq n \leq
			    N_{\rm antennas}-1 \f$
    */
#ifdef HAVE_CASA
    void setAntPosition (const casa::Vector<double> &antPosition,
			 const uint &number);
#else
#ifdef HAVE_BLITZ
    void setAntPosition (const blitz::Array<double,1> &antPosition,
			 const uint &number);
#endif
#endif
    
    /*!
      \brief Get the sky positions, for which the delay is computed

      \return skyPosition -- The positions on the sky, for which the delay is
                             computed, [nofSkyPositions,3].
    */
#ifdef HAVE_CASA
    inline casa::Matrix<double> skyPositions () {
      return skyPositions_p;
    }
#else
#ifdef HAVE_BLITZ
    inline blitz::Array<double,2> skyPositions () {
      return skyPositions_p;
    }
#endif
#endif

    /*!
      \brief Set the sky positions, for which the delay is computed
      
      \param skyPositions -- The source positions, for which the delay is
                             computed, [nofSkyPositions,3]; cartesian
			     coordinates by default
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setSkyPositions (const casa::Matrix<double> &skyPositions,
			  const bool &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (const blitz::Array<double,2> &skyPositions,
			  const bool &bufferDelays=false);
#endif
#endif

    /*!
      \brief Set the sky positions, for which the delay is computed
      
      \param skyPositions -- The sky positions, for which the delay is computed,
                             [nofSources,3]
      \param coordType    -- Coordinate type, as which the position is provided;
                             if position not provided in cartesian coordinates,
			     transformation will be performed
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
    */
#ifdef HAVE_CASA
    bool setSkyPositions (casa::Matrix<double> const &skyPositions,
			  CR::CoordinateTypes const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (blitz::Array<double,2> const &skyPositions,
			  CR::CoordinateTypes const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#endif
#endif
    
    /*!
      \brief 
      
      \param xValues         -- 
      \param yValues         -- 
      \param zValues         -- 
      \param coordType       -- Coordinate type, as which the position is
                                provided; if position not provided in cartesian
				coordinates, transformation will be performed
      \param anglesInDegrees -- 
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.
      
      \return status -- Success or failure of the operation; returns
                        <t>false</tt> if an error was encountered
    */
#ifdef HAVE_CASA
    bool setSkyPositions (casa::Vector<double> const &xValues,
			  casa::Vector<double> const &yValues,
			  casa::Vector<double> const zValues,
			  CR::CoordinateTypes const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#else
#ifdef HAVE_BLITZ
    bool setSkyPositions (blitz::Array><double,1> const &xValues,
			  blitz::Array><double,1> const &yValues,
			  blitz::Array><double,1> const zValues,
			  CR::CoordinateTypes const &coordType,
			  bool const &anglesInDegrees=false,
			  bool const &bufferDelays=false);
#endif
#endif

    /*!
      \brief Get the values of the geometrical delay

      \return delay -- [nofAntennas,nofSkyPositions] The geometrical delays for
                       for the combination of antenna and sky positions
    */
#ifdef HAVE_CASA
    casa::Matrix<double> delays ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> delays ();
#endif
#endif

    /*!
      \brief Geometrical delay for a combination of antenna and sky positions

      \param antPositions -- The antPositions, for which the delay is computed;
                             values are given as [nofAntenas,nofCoordinates] with
			     the antenna positions given in cartesian coordinates
			     \f$ (x,y,z) \f$
      \param skyPositions -- The sky positions, for which the delay is computed
      \param bufferDelays  -- Buffer the values for the geometrical delay? If set
                             <i>yes</i> the delays will be computed from the 
			     provided antenna and sky positions and afterwards
			     kept in memory; if set <i>no</i> only the input 
			     parameters are stored an no further action is taken.

      \return delay -- [nofAntennas,nofSkyPositions] The geometrical delays for
                       for the combination of antenna and sky positions
    */
#ifdef HAVE_CASA
    casa::Matrix<double> delays (const casa::Matrix<double> &antPositions,
				const casa::Matrix<double> &skyPositions,
				const bool &bufferDelays=false)
      {
	bufferDelays_p = bufferDelays;
	antPositions_p = antPositions;
	skyPositions_p = skyPositions;
	return delays ();
      }
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> delays (const blitz::Array<double,2> &antPositions,
				  const blitz::Array<double,2> &skyPositions,
				  const bool &bufferDelays=false)
      {
	bufferDelays_p = bufferDelays;
	antPositions_p = antPositions;
	skyPositions_p = skyPositions;
	return delays ();
      }
#endif
#endif
    
    /*!
      \brief Get the number of baselines
      
      \return nofBaselines -- The number of baselines, \f$ N_{\rm Baselines} \f$,
                              which can be computed from the antenna positions:
			      \f$ N_{\rm Baselines} = \frac{1}{2} (N^2-N)
			      = \frac{N}{2} (N-1) \f$
    */
#ifdef HAVE_CASA
    inline int nofBaselines () {
      return antPositions_p.nrow()*(antPositions_p.nrow()-1)/2;
    }
#else
#ifdef HAVE_BLITZ
    inline int nofBaselines () {
      return antPositions_p.rows()*(antPositions_p.rows()-1)/2;
    }
#endif
#endif

    /*!
      \brief Get the number of antenna positions

      \return nofAntennaPositions -- The number of antenna positions, for which
                                     the geometrical delay is computed.
    */
#ifdef HAVE_CASA
    inline int nofAntennaPositions () {
      return antPositions_p.nrow();
    }
#else
#ifdef HAVE_BLITZ
    inline int nofAntennaPositions () {
      return antPositions_p.rows();
    }
#endif
#endif

    /*!
      \brief Get the number of (source) positions

      \return nofPositions -- The number of (source) positions, which which the
                              delays are computed; corresponds to the number of
			      elements along the first axis of the
			      <tt>positions</tt> array.
    */
#ifdef HAVE_CASA
    inline int nofSkyPositions () {
      return skyPositions_p.nrow();
    }
#else
#ifdef HAVE_BLITZ
    inline int nofSkyPositions () {
      return skyPositions_p.rows();
    }
#endif
#endif
    
    // ------------------------------------------------------------------ Methods
    
    /*
      \brief Compute an individual delay
    */
    double calcDelay (double const &xSky,
		      double const &ySky,
		      double const &zSky,
		      double const &xAntenna,
		      double const &yAntenna,
		      double const &zAntenna);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeometricalDelay.
    */
    inline std::string className () const {
      return "GeometricalDelay";
    }

    /*!
      \brief Provide a summary of the objects status and contents
    */
    inline void summary () {
      summary (std::cout);
    }
    
    /*!
      \brief Provide a summary of the objects status and contents

      \param os -- Output stream to which the summary is written
    */
    void summary (std::ostream &os);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeometricalDelay const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Store the geometrical delays in the internal buffer
    */
    void setDelays ();
    
    /*!
      \brief Compute the geometrical delays

      \return delays -- The geometrical delays, [nofAntennas,nofPositions]
    */
#ifdef HAVE_CASA
    casa::Matrix<double> calcDelays ();
#else
#ifdef HAVE_BLITZ
    blitz::Array<double,2> calcDelays ();
#endif
#endif

  };  // CLASS GEOMETRICALDELAY -- END
  
}  // NAMESPACE CR -- END

#endif /* GEOMETRICALDELAY_H */
  
