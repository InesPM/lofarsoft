/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef TABULARCOORDINATE_H
#define TABULARCOORDINATE_H

// DAL header files
#include "CoordinateInterface.h"

#ifdef HAVE_CASA
#include <coordinates/Coordinates/TabularCoordinate.h>
#endif

namespace DAL {  // Namespace DAL -- begin
  
  /*!
    \class TabularCoordinate
    
    \ingroup DAL
    \ingroup coordinates
    
    \brief Brief description for class TabularCoordinate
    
    \author Lars B&auml;hren

    \date 2009/06/24

    \test tTabularCoordinate.cpp

    <h3>Prerequisite</h3>

    <ul type="square">
      <li>[start filling in your text here]
    </ul>

    <h3>Synopsis</h3>

    <h3>Example(s)</h3>

    <ul>
      <li>One of the cases where a tabular axis can become of use is for a
      non-contiguous time-axis (e.g. due to the fact that parts of an observation
      were dropped during transfer).
      \verbatim
      COORDINATE_TYPE = Tabular
      NOF_AXES        = 1
      AXIS_NAMES      = "Time"
      AXIS_UNITS      = "s"
      PIXEL_VALUES    = [0,1,2,3,4,5,6,7]
      WORLD_VALUES    = [0,1,2,5,10,20,50,100]
      \endverbatim
    </ul>
    
  */
  class TabularCoordinate : public CoordinateInterface {
    
  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    TabularCoordinate ();
    //! Argumented constructor
    TabularCoordinate (std::string const &axisNames,
		       std::string const &axisUnits,
		       std::vector<double> const &pixelValues,
		       std::vector<double> const &worldValues);
    //! Copy constructor
    TabularCoordinate (TabularCoordinate const &other);
    
    // === Destruction ==========================================================
    
    //! Destructor
    ~TabularCoordinate ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TabularCoordinate object from which to make a copy.
    */
    TabularCoordinate& operator= (TabularCoordinate const &other);
    
    // === Parameter access =====================================================
    
    //! Get the pixel values
    inline std::vector<double> pixelValues () {
      return pixelValues_p;
    }
    //! Set the pixel values
    bool setPixelValues (std::vector<double> const &pixelValues);
    //! Get the world values
    inline std::vector<double> worldValues () {
      return worldValues_p;
    }
    //! Set the world values
    bool setWorldValues (std::vector<double> const &worldValues);
    //! Set the values along the pixel and world axis
    bool setAxisValues (std::vector<double> const &pixelValues,
			std::vector<double> const &worldValues);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TabularCoordinate.
    */
    inline std::string className () const {
      return "TabularCoordinate";
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
    
    // === Public methods =======================================================

#ifdef HAVE_HDF5    
    //! Write the coordinate object to a HDF5 file
    void write_hdf5 (hid_t const &groupID);
    
    //! Write the coordinate object to a HDF5 file
    void write_hdf5 (hid_t const &locationID,
		  std::string const &name);
    
    //! Read the coordinate object from a HDF5 file
    void read_hdf5 (hid_t const &groupID);
    
    //! Read the coordinate object from a HDF5 file
    void read_hdf5 (hid_t const &locationID,
		    std::string const &name);
#endif
    
#ifdef HAVE_CASA
    //! Create coordinate from casa::Coordinate object
    void importCoordinate (casa::TabularCoordinate const &coord);
    //! Create casa::Coordinate object from coordinate parameters
    void exportCoordinate (casa::TabularCoordinate &coord);
#endif

  private:
    
    //! Initialize internal parameters
    void init (std::string const &axisNames="UNDEFINED",
	       std::string const &axisUnits="UNDEFINED");

    //! Initialize internal parameters
    void init (std::vector<std::string> const &axisNames,
	       std::vector<std::string> const &axisUnits,
	       std::vector<double> const &pixelValues,
	       std::vector<double> const &worldValues);
    
    //! Set the attributes attached to the coordinate
    inline void setAttributes ()
    {
      attributes_p.insert("PIXEL_VALUES");
      attributes_p.insert("WORLD_VALUES");
    }
    //! Unconditional copying
    void copy (TabularCoordinate const &other);
    
    //! Unconditional deletion
    void destroy(void);
    
  }; // Class TabularCoordinate -- end
  
} // Namespace DAL -- end

#endif /* TABULARCOORDINATE_H */

