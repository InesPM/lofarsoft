/**************************************************************************
 *  Header file of Vector module for CR Pipeline Python bindings          *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
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

#ifndef CR_PIPELINE_VECTOR_H
#define CR_PIPELINE_VECTOR_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

using namespace std;

// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  namespace Vector { // Namespace Vector -- begin


    template <class Iter, class IterIn>
      void hCopy(const Iter vecout, const Iter vecout_end, const IterIn vecin, const IterIn vecin_end)
    {
      typedef IterValueType T;
      Iter itout(vecout);
      IterIn itin(vecin);
      if (itin >= vecin_end) return;
      while (itout != vecout_end) {
        *itout=hfcast<T>(*itin);
        ++itin; ++itout;
        if (itin==vecin_end) itin=vecin;
      };
    }

    template <class Iter, class Iterin, class IterI>
      void hCopy(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end, const IterI index, const IterI index_end, const HInteger number_of_elements)
    {
      typedef IterValueType T;
      Iter itout(vecout);
      IterI itidx(index);
      Iterin itin;
      HInteger count(number_of_elements);
      if ((index >= index_end) || count==0) return;
      if (vecin >= vecin_end) return;
      if (count<0) count=(vecout_end-vecout);
      while (itout != vecout_end) {
        itin = vecin + *itidx;
        if (itin < vecin_end) *itout=hfcast<T>(*itin);
        --count; if (count==0) return;
        ++itidx; ++itout;
        if (itidx==index_end) itidx=index;
      };
    }


  } // Namespace Vector -- end

} // Namespace PyCR -- end



// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mVector.def.h"

#endif /* CR_PIPELINE_VECTOR_H */
