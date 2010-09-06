/**************************************************************************
 *  This file is part of the Transient Template Library.                  *
 *  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        * 
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef TTL_COORDINATES_H
#define TTL_COORDINATES_H

// SYSTEM INCLUDES
#include <string>
#include <iostream> // NOTE remove this

// PROJECT INCLUDES
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <measures/Measures/MEpoch.h>
#include <tmf.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

// DOXYGEN TAGS
/*!
  \file coordinates.h
  \ingroup CR
  \ingroup CR_TTL
 */

namespace ttl
{
  namespace coordinates
  {
    /*!
      \brief Get world coordinates for given vector of pixel coordinates

      \param world begin iterator for world coordinate array
      \param world_end end iterator for world coordinate array
      \param pixel begin iterator for pixel coordinate array
      \param pixel_end end iterator for pixel coordinate array
      \param refcode reference code for coordinate system e.g. AZEL,J2000,...
      \param projection the projection used e.g. SIN,STG,...
      \param refLong reference value for longtitude (CRVAL)
      \param refLat reference value for latitude (CRVAL)
      \param incLon increment value for longtitude (CDELT)
      \param incLat increment value for latitude (CDELT)
      \param refX reference x pixel (CRPIX)
      \param refY reference y pixel (CRPIX)
     */
    template <class DIter>
      bool pixel2World(const DIter world, const DIter world_end,
          const DIter pixel, const DIter pixel_end,
          const std::string refcode, const std::string projection,
          const double refLong, const double refLat,
          const double incLong, const double incLat,
          const double refX, const double refY
          )
      {
        // Check input
        if (world_end-world != pixel_end-pixel)
        {
          std::cerr<<"Error, in pixel2World(): input and output vector not of same length."<<std::endl;
          return false;
        }

        casa::MDirection::Types type;
        if (casa::MDirection::getType(type, refcode) != true)
        {
          std::cerr<<"Error, in pixel2World(): input reference type invalid."<<std::endl;
          return false;
        }

        casa::Projection::Type proj;
        proj=casa::Projection::type(static_cast<casa::String>(projection));
        if (proj==casa::Projection::N_PROJ)
        {
          std::cerr<<"Error, in pixel2World(): input projection type invalid."<<std::endl;
        }

        // Get spatial direction coordinate system
        casa::Matrix<casa::Double> xform(2,2);
        xform = 0.0; xform.diagonal() = 1.0;

        casa::DirectionCoordinate dir(type,
            proj,
            static_cast<casa::Double>(refLong),
            static_cast<casa::Double>(refLat),
            static_cast<casa::Double>(incLong),
            static_cast<casa::Double>(incLat),
            xform,
            static_cast<casa::Double>(refX),
            static_cast<casa::Double>(refY));

        //        std::cout<<casa::MDirection::showType(dir.directionType())<<"\t"<<dir.projection().name()<<std::endl;

        // Get iterators
        DIter world_it=world;
        DIter pixel_it=pixel;

        // Placeholders for conversion
        casa::Vector<casa::Double> cworld(2), cpixel(2); 

        // Loop over all pixels
        while (world_it!=world_end && pixel_it!=pixel_end)
        {
          // Get pixel coordinates into casa vector for conversion
          cpixel[0]=static_cast<casa::Double>(*pixel_it);
          ++pixel_it;
          cpixel[1]=static_cast<casa::Double>(*pixel_it);

          // Convert pixel to world coordinates
          dir.toWorld(cworld, cpixel);

          // Retrieve world coordinates
          *world_it=static_cast<double>(cworld[0]);
          ++world_it;
          *world_it=static_cast<double>(cworld[1]);

          // Next pixel
          ++pixel_it;
          ++world_it;
        }

        // Conversion successfull
        return true;
      }

    /*!
      \brief Get pixel coordinates for given vector of world coordinates

      \param pixel begin iterator for pixel coordinate array
      \param pixel_end end iterator for pixel coordinate array
      \param world begin iterator for world coordinate array
      \param world_end end iterator for world coordinate array
      \param refcode reference code for coordinate system e.g. AZEL,J2000,...
      \param projection the projection used e.g. SIN,STG,...
      \param refLong reference value for longtitude (CRVAL)
      \param refLat reference value for latitude (CRVAL)
      \param incLon increment value for longtitude (CDELT)
      \param incLat increment value for latitude (CDELT)
      \param refX reference x pixel (CRPIX)
      \param refY reference y pixel (CRPIX)
     */
    template <class DIter>
      bool world2Pixel(const DIter pixel, const DIter pixel_end,
          const DIter world, const DIter world_end,
          const std::string refcode, const std::string projection,
          const double refLong, const double refLat,
          const double incLong, const double incLat,
          const double refX, const double refY
          )
      {
        // Check input
        if (world_end-world != pixel_end-pixel)
        {
          std::cerr<<"Error, in world2Pixel(): input and output vector not of same length."<<std::endl;
          return false;
        }

        casa::MDirection::Types type;
        if (casa::MDirection::getType(type, refcode) != true)
        {
          std::cerr<<"Error, in world2Pixel(): input reference type invalid."<<std::endl;
          return false;
        }

        casa::Projection::Type proj;
        proj=casa::Projection::type(static_cast<casa::String>(projection));
        if (proj==casa::Projection::N_PROJ)
        {
          std::cerr<<"Error, in world2Pixel(): input projection type invalid."<<std::endl;
        }

        // Get spatial direction coordinate system
        casa::Matrix<casa::Double> xform(2,2);
        xform = 0.0; xform.diagonal() = 1.0;

        casa::DirectionCoordinate dir(type,
            proj,
            static_cast<casa::Double>(refLong),
            static_cast<casa::Double>(refLat),
            static_cast<casa::Double>(incLong),
            static_cast<casa::Double>(incLat),
            xform,
            static_cast<casa::Double>(refX),
            static_cast<casa::Double>(refY));

        //        std::cout<<casa::MDirection::showType(dir.directionType())<<"\t"<<dir.projection().name()<<std::endl;

        // Get iterators
        DIter world_it=world;
        DIter pixel_it=pixel;

        // Placeholders for conversion
        casa::Vector<casa::Double> cworld(2), cpixel(2); 

        // Loop over all world coordinates 
        while (world_it!=world_end && pixel_it!=pixel_end)
        {
          // Get world coordinates into casa vector for conversion
          cworld[0]=static_cast<casa::Double>(*world_it);
          ++world_it;
          cworld[1]=static_cast<casa::Double>(*world_it);

          // Convert world to pixel coordinates
          dir.toPixel(cpixel, cworld);

          // Retrieve pixel coordinates
          *pixel_it=static_cast<double>(cpixel[0]);
          ++pixel_it;
          *pixel_it=static_cast<double>(cpixel[1]);

          // Next pixel
          ++pixel_it;
          ++world_it;
        }

        // Conversion successfull
        return true;
      }

    /*!
      \brief Given an array of (az, el, r, az, el, r, ...) coordinates
      return an array of (x, y, z, x, y, z, ...) coordinates.
     */
    template <class DIter>
      void azElRadius2Cartesian(DIter out, DIter out_end,
          DIter in, DIter in_end,
          bool anglesInDegrees)
      {
        double deg2rad = (3.14159265359/180.);

        // Get iterators
        DIter out_it=out;
        DIter in_it=in;

        // Loop over coordinates
        if (anglesInDegrees) {
          while (out_it!=out_end && in_it!=in_end)
          {
            *out_it     = *(in_it+2)*cos(*(in_it+1) * deg2rad)*sin(*in_it * deg2rad);
            *(out_it+1) = *(in_it+2)*cos(*(in_it+1) * deg2rad)*cos(*in_it * deg2rad);
            *(out_it+2) = *(in_it+2)*sin(*(in_it+1) * deg2rad);

            in_it=in_it+3;
            out_it=out_it+3;
          }
        } else {
          while (out_it!=out_end && in_it!=in_end)
          {
            *out_it     = *(in_it+2)*cos(*(in_it+1))*sin(*in_it);
            *(out_it+1) = *(in_it+2)*cos(*(in_it+1))*cos(*in_it);
            *(out_it+2) = *(in_it+2)*sin(*(in_it+1));

            in_it=in_it+3;
            out_it=out_it+3;
          }
        };
      }

    /*!
      \brief Convert array of equatorial J2000 coordinates to horizontal,
      AZEL coordinates.

      \param hc array with horizontal coordiates (alt, az, alt, az, ...)
      \param ec array with equatorial coordinates (ra, dec, ra, dec, ...) 
      \param utc UTC as Julian Day
      \param ut1_utc difference UT1-UTC (as obtained from IERS bullitin A)
             if 0 a maximum error of 0.9 seconds is made.
      \param L longitude of telescope
      \param phi latitude of telescope
     */
    template <class DIter>
      void equatorial2Horizontal(const DIter hc, const DIter hc_end,
          const DIter ec, const DIter ec_end,
          const double utc, const double ut1_utc,
          const double L, const double phi)
      {
        // Constants
        const int SECONDS_PER_DAY = 24 * 3600;

        // Variables
        double alpha, delta, A, h, H;

        // Calculate Terestrial Time (TT)
        const double tt = utc + tmf::tt_utc(utc) / SECONDS_PER_DAY;

        // Calculate Universal Time (UT1)
        const double ut1 = utc + ut1_utc / SECONDS_PER_DAY;

        // Calculate Local Apparant Sidereal Time (LAST)
        const double theta_L = tmf::last(ut1, tt, L);

        // Get iterators
        DIter hc_it=hc;
        DIter ec_it=ec;

        while (hc_it!=hc_end && ec_it!=ec_end)
        {
          // Get equatorial coordinates
          alpha = *ec_it;
          ++ec_it;
          delta = *ec_it;

          // Calculate hour angle
          H = tmf::rad2circle(theta_L - alpha);

          // Convert from equatorial to horizontal coordinates
          tmf::equatorial2horizontal(A, h, H, delta, phi);

          // Store output
          *hc_it = A;
          ++hc_it;
          *hc_it = h;

          // Advance iterator
          ++hc_it;
          ++ec_it;
        }
      }
  } // End coordinates
} // End ttl

#endif // TTL_COORDINATES_H

