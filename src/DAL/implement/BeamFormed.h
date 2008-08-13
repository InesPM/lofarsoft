/*-------------------------------------------------------------------------*
 | $Id:: dal.h 1126 2007-12-10 17:14:20Z masters                         $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007 by Joseph Masters                                  *
 *   jmasters@science.uva.nl                                               *
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

#ifndef BEAMFORMED_H
#define BEAMFORMED_H

#ifndef BEAMGROUP_H
#include <BeamGroup.h>
#endif

namespace DAL
  {

  /*!
     \class BeamFormed

     \ingroup DAL

     \brief High-level interface between beam-formed data and the DAL

     \author Joseph Masters
   */

  class BeamFormed
    {

    private:

      // Filename of the dataset
      std::string filename_p;

      // HDF5 file handle ID
      hid_t H5fileID_p;

      // DAL Dataset object to handle the basic I/O
      dalDataset * dataset_p;

      bool init();

      // Vector of beam groups within the dataset
      std::vector<BeamGroup*> beamGroups_p;

      bool status;

    public:

      BeamFormed();
      BeamFormed(std::string const &filename);
      ~BeamFormed();


      // ---------------------------------------------------------- print_vector

      /*!
        \brief Print a list of vector elements
        \param os        -- output stream [I]
        \param vec       -- vector [I]
      */
      template<class T>
      void print_vector ( std::ostream& os,
                          std::vector<T> &vec)
      {
        for (uint n(0); n<vec.size(); n++)
          {
            os << vec[n] << ", ";
          }
      }

      std::vector< std::string > h5get_str_array_attr( std::string attrname,
          hid_t obj_id );
      /*! \brief Provide a summary of the internal status */
      inline void summary ()
      {
        summary (cout, true);
      }
      void summary (ostream &os, bool const &listBeams);
      std::vector<std::string> beams();
      BeamGroup * getBeam( int beam );
      std::string filename();
      std::string telescope();
      int nstations();
      std::string datatype();
      std::string emband();
      std::vector<std::string> sources();
      std::string observation_id();
      std::string proj_id();
      std::string point_ra();
      std::string point_dec();
      std::string observer();
      std::string epoch_mjd();
      std::string epoch_date();
      std::string epoch_utc();
      std::string epoch_lst();
      int main_beam_diam();
      int center_freq();
      int bandwidth();
      double total_integration_time();
      int breaks();
      int dispersion_measure();
      int number_of_samples();
      double sampling_time();
      int number_of_beams();
      int sub_beam_diameter();
      int weather_temperature();
      int weather_humidity();
      std::vector<int> station_temperatures();
      std::string notes();

      /************************************************************************
       *
       * The following functions are boost wrappers to allow some previously
       *   defined functions to be easily called from a python prompt.
       *
       ************************************************************************/
#ifdef PYTHON

      void summary_boost();
      bpl::list beams_boost();
      bpl::list source_boost();

      // ---------------------------------------------------------- vector2list

      /* utility to turn a vector into a python list */

      template <class T>
      bpl::list vector2list( std::vector<T> vec )
      {
        bpl::list mylist;

        typename std::vector<T>::iterator it;
        for ( it=vec.begin(); it < vec.end(); it++ )
          mylist.append( *it );

        return mylist;
      }

#endif // PYTHON

    }; // BeamFormed class

} // namespace DAL

#endif // BEAMFORMED_H
