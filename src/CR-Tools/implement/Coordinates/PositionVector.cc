/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <Coordinates/PositionVector.h>

namespace CR { // Namespace  -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  PositionVector::PositionVector ()
  {;}
  
  PositionVector::PositionVector (PositionVector const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  PositionVector::~PositionVector ()
  {
    destroy();
  }
  
  void PositionVector::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  PositionVector& PositionVector::operator= (PositionVector const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void PositionVector::copy (PositionVector const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void PositionVector::summary (std::ostream &os)
  {
    os << "[PositionVector] Summary of internal parameters." << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                      convert
  
  bool PositionVector::convert (casa::Vector<double> &out,
				CR::CoordinateType::Types const &typeOut,
				casa::Vector<double> const &in,
				CR::CoordinateType::Types const &typeIn,
				bool const &anglesInDegrees)
  {
    bool status (true);

    CR::PositionVector vec;
    
    switch (typeOut) {
      //____________________________________________________
      //                            Conversions -> Cartesian
    case CoordinateType::Cartesian:
      switch (typeIn) {
      case CoordinateType::Cartesian:
	out = in;
	break;
      case CoordinateType::Spherical:
	vec.Spherical2Cartesian (out(0), out(1), out(2),
				 in(0) ,  in(1),  in(2),
				 anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	vec.Cylindrical2Cartesian (out(0), out(1), out(2),
				   in(0) ,  in(1),  in(2),
				   anglesInDegrees);
	break;
      case CoordinateType::AzElRadius:
	vec.AzElRadius2Cartesian (out(0), out(1), out(2),
				  in(0) ,  in(1),  in(2),
				  anglesInDegrees);
	break;
      case CoordinateType::LongLatRadius:
	vec.LongLatRadius2Cartesian (out(0), out(1), out(2),
				     in(0) ,  in(1),  in(2),
				     anglesInDegrees);
	break;
      default:
	std::cerr << "[PositionVector::convert]"
		  << " Requested conversion not yet implemented!" << std::endl;
	status = false;
	break;
      };
      break;
      //____________________________________________________
      //                            Conversions -> Spherical
    case CoordinateType::Spherical:
      switch (typeIn) {
      case CoordinateType::Cartesian:
	vec.Cartesian2Spherical (out(0), out(1), out(2),
				 in(0) ,  in(1),  in(2),
				 anglesInDegrees);
	break;
      case CoordinateType::Spherical:
	out = in;
	break;
      case CoordinateType::AzElRadius:
	vec.AzElRadius2Spherical (out(0), out(1), out(2),
				  in(0) ,  in(1),  in(2),
				  anglesInDegrees);
	break;
      case CoordinateType::LongLatRadius:
	vec.LongLatRadius2Spherical (out(0), out(1), out(2),
				     in(0) ,  in(1),  in(2),
				     anglesInDegrees);
	break;
      default:
	std::cerr << "[PositionVector::convert]"
		  << " Requested conversion not yet implemented!" << std::endl;
	status = false;
	break;
      };
      break;
    default:
      std::cerr << "[PositionVector::convert]"
		<< " Requested conversion not yet implemented!" << std::endl;
      status = false;
      break;
    };
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                      Conversion -> Cartesian

  void PositionVector::Spherical2Cartesian (double &out_x,
					    double &out_y,
					    double &out_z,
					    double const &in_r,
					    double const &in_phi,
					    double const &in_theta,
					    bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      Spherical2Cartesian (out_x,out_y,out_z,
			   in_r,deg2rad(in_phi),deg2rad(in_theta),
			   false);
    } else {
      out_x = in_r*sin(in_theta)*cos(in_phi);
      out_y = in_r*sin(in_theta)*sin(in_phi);
      out_z = in_r*cos(in_theta); 
    }
  }
  
  void PositionVector::Cylindrical2Cartesian (double &out_x,
					      double &out_y,
					      double &out_z,
					      double const &in_rho,
					      double const &in_phi,
					      double const &in_z,
					      bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      Cylindrical2Cartesian (out_x,out_y,out_z,
			     in_rho,deg2rad(in_phi),in_z,
			     false);
    } else {
      out_x = in_rho*cos(in_phi);
      out_y = in_rho*sin(in_phi);
      out_z = in_z;
    }
  }
  
  void PositionVector::AzElRadius2Cartesian (double &out_x,
					     double &out_y,
					     double &out_z,
					     double const &in_az,
					     double const &in_el,
					     double const &in_r,
					     bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      AzElRadius2Cartesian (out_x,out_y,out_z,
			    deg2rad(in_az),deg2rad(in_el),in_r,
			    false);
    } else {
      out_x = in_r*cos(in_el)*sin(in_az);
      out_y = in_r*cos(in_el)*cos(in_az);
      out_z = in_r*sin(in_el);
    }
  }
  
  void PositionVector::LongLatRadius2Cartesian (double &out_x,
						double &out_y,
						double &out_z,
						double const &in_long,
						double const &in_lat,
						double const &in_r,
						bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      LongLatRadius2Cartesian (out_x,out_y,out_z,
			       deg2rad(in_long),deg2rad(in_lat),in_r,
			       false);
    } else {
      out_x = in_r*cos(in_lat)*cos(in_long);
      out_y = in_r*cos(in_lat)*sin(in_long);
      out_z = in_r*sin(in_lat);
    }
  }
  
  //_____________________________________________________________________________
  //                                                     Conversions -> Spherical
  
  void PositionVector::Cartesian2Spherical (double &r,
					    double &phi,
					    double &theta,
					    double const &x,
					    double const &y,
					    double const &z,
					    bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      r     = sqrt(x*x+y*y+z*z);
      phi   = rad2deg(atan2(y,x));
      theta = rad2deg(acos(z/r));
    } else {
      r     = sqrt(x*x + y*y + z*z);
      phi   = atan2(y,x);
      theta = acos(z/r);
    }

    if (r == 0) {
      theta = 0;
    }

  }
  
  void PositionVector::AzElRadius2Spherical (double &out_r,
					     double &out_phi,
					     double &out_theta,
					     double const &in_az,
					     double const &in_el,
					     double const &in_r,
					     bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_r     = in_r;
      out_phi   = 90-in_az;
      out_theta = 90-in_el;
    } else {
      out_r     = in_r;
      out_phi   = deg2rad(90)-in_az;
      out_theta = deg2rad(90)-in_el;
    }
  }

  void PositionVector::LongLatRadius2Spherical (double &out_r,
						double &out_phi,
						double &out_theta,
						double const &in_long,
						double const &in_lat,
						double const &in_r,
						bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_r     = in_r;
      out_phi   = in_long;
      out_theta = 90-in_lat;
    } else {
      out_r     = in_r;
      out_phi   = in_long;
      out_theta = deg2rad(90)-in_lat;
    }
  }
  
  //_____________________________________________________________________________
  //                                                   Conversions -> Cylindrical


  //_____________________________________________________________________________
  //                                                    Conversions -> AzElRadius


  //_____________________________________________________________________________
  //                                                 Conversions -> LongLatRadius

  
} // Namespace  -- end
