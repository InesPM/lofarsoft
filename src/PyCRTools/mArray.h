/**************************************************************************
 *  Header file of Array module for CR Pipeline Python bindings.          *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
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

#ifndef CR_PIPELINE_ARRAY_H
#define CR_PIPELINE_ARRAY_H

#include "core.h"
#include "mModule.h"


//========================================================================
//                             Array Class
//========================================================================

template <class T>
  class hArray {

  private:

  struct unit_container {
    HNumber scale_factor;
    HString prefix;
    HString name;
    std::map<HString,HNumber> prefix_to_factor;
  };

  struct storage_container {
    HInteger refcount;
    std::vector<HInteger> * dimensions_p;
    std::vector<HInteger> * slice_sizes_p;
    std::vector<T>* vec_p;
    unit_container unit;
    bool trackHistory;
    std::vector<HString> history;
    std::map<HString,HString> keywords;
    std::map<HString,HPyObjectPtr> keywords_py;
  };

    storage_container * storage_p;
    HInteger slice_begin, slice_end, slice_size;
    HInteger loop_slice_begin, loop_slice_end, loop_slice_size, loop_lower_level_size;
    HInteger loop_start, loop_end, loop_increment, loop_i, loop_nslice, loop_maxn;
    HInteger subslice_level,subslice_start,subslice_end;
    std::vector<HInteger> subslice_vec_start,subslice_vec_end;
    std::vector<HInteger>::iterator subslice_start_it,subslice_end_it;
    std::vector<HInteger> index_vector;
    bool loop_over_indexvector;
    bool loop_next;
    bool doiterate;

    hArray(storage_container * sptr);
    void init();
    void new_storage();
    void calcSizes();

  public:

    hArray();
    hArray(std::vector<T> & vec);
    ~hArray();
    hArray<T> & shared_copy();
    hArray<T> & resize(HInteger newsize);
    hArray<T> & setVector(std::vector<T> & vec);
    std::vector<T> & getVector();
    std::vector<HInteger> & getSizes();
    boost::python::tuple shape();
    void reshapeList(const boost::python::list & dims);
    void reshapeTuple(const boost::python::tuple & dims);
    HInteger getNumberOfDimensions();
    hArray<T> & setSlice(HInteger beg, HInteger end=-1);
    hArray<T> & setSliceVector(std::vector<HInteger> & index_vector);
    HInteger getSubSliceEnd();
    HInteger getSubSliceStart();
    hArray<T> & setSubSlice(HInteger start, HInteger end, HInteger level);
    hArray<T> & setSubSliceVec(std::vector<HInteger> & start, std::vector<HInteger> & end, HInteger level);
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    HInteger getLoop_i();
    HInteger getLoop_nslice();
    HInteger getLoop_start();
    HInteger getLoop_end();
    HInteger loop_slice_start_offset;
    HInteger loop_slice_end_offset;
    HInteger getLoop_increment();
    HInteger getBegin();
    HInteger getEnd();
    HInteger getSize();
    HInteger length();
    bool loopingMode();
    bool doLoopAgain();
    hArray<T> & loop(std::vector<HInteger> & start_element_index, HInteger start=0, HInteger end=-1, HInteger increment=1);
    hArray<T> & loopVector(std::vector<HInteger> & start_element_index, std::vector<HInteger> & vec);
    HInteger setLoopSlice(std::vector<HInteger> & start_element_index);
    hArray<T> &  next();
    hArray<T> &  loopOn();
    hArray<T> &  loopOff();
    hArray<T> &  resetLoop();
    hArray<T> &  all();
    hArray<T> &  setUnit(HString prefix, HString name);
    hArray<T> &  clearUnit();
    HString  getUnit();
    HString  getUnitName();
    HString  getUnitPrefix();
    hArray<T> &  setHistory(bool on_or_off);
    bool isTrackingHistory();
    hArray<T> &  clearHistory();
    hArray<T> &  addHistory(HString name, HString text);
    std::vector<HString> & getHistory();
    void printHistory();
    HString getKey(HString key);
    hArray<T> & setKey(HString key, HString contents);
    HString writeRaw();
    void readRaw(HString raw);

  };

// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

#define HFPP_hARRAY_BOOST_PYTHON_WRAPPER(TYPE,NAME)	\
    class_<hArray<TYPE> >(#NAME) \
    .enable_pickling() \
    .def("getVector",&hArray<TYPE>::getVector,return_internal_reference<>()) \
    .def("shared_copy",&hArray<TYPE>::shared_copy,return_internal_reference<>()) \
    .def("shape",&hArray<TYPE>::shape) \
    .def("reshape",&hArray<TYPE>::reshapeList) \
    .def("reshape",&hArray<TYPE>::reshapeTuple) \
    .def("getSizes",&hArray<TYPE>::getSizes,return_internal_reference<>()) \
    .def("setVector",&hArray<TYPE>::setVector,return_internal_reference<>())\
    .def("setSlice",&hArray<TYPE>::setSlice,return_internal_reference<>())				\
    .def("setSliceVector",&hArray<TYPE>::setSliceVector,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSlice,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSliceVec,return_internal_reference<>())				\
    .def("getNumberOfDimensions",&hArray<TYPE>::getNumberOfDimensions)	\
    .def("getBegin",&hArray<TYPE>::getBegin)				\
    .def("getEnd",&hArray<TYPE>::getEnd)				\
    .def("getSize",&hArray<TYPE>::getSize)				\
    .def("loopingMode",&hArray<TYPE>::loopingMode)			\
    .def("doLoopAgain",&hArray<TYPE>::doLoopAgain)			\
    .def("loop_i",&hArray<TYPE>::getLoop_i)				\
    .def("loop_nslice",&hArray<TYPE>::getLoop_nslice)			\
    .def("setLoopSlice",&hArray<TYPE>::setLoopSlice)			\
    .def("loop_start",&hArray<TYPE>::getLoop_start)			\
    .def("loop_end",&hArray<TYPE>::getLoop_end)				\
    .def("loop_increment",&hArray<TYPE>::getLoop_increment)		\
    .def("__len__",&hArray<TYPE>::length)				\
    .def("resize",&hArray<TYPE>::resize,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loop,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loopVector,return_internal_reference<>())					\
    .def("resetLoop",&hArray<TYPE>::resetLoop,return_internal_reference<>())				\
    .def("noOn",&hArray<TYPE>::loopOn,return_internal_reference<>())				\
    .def("noOff",&hArray<TYPE>::loopOff,return_internal_reference<>())				\
    .def("next",&hArray<TYPE>::next,return_internal_reference<>())       \
    .def("setUnit_",&hArray<TYPE>::setUnit,return_internal_reference<>())	\
    .def("getUnit",&hArray<TYPE>::getUnit)	\
    .def("getUnitName",&hArray<TYPE>::getUnitName)	\
    .def("getUnitPrefix",&hArray<TYPE>::getUnitPrefix)	\
    .def("clearUnit",&hArray<TYPE>::clearUnit,return_internal_reference<>())	\
    .def("addHistory",&hArray<TYPE>::addHistory,return_internal_reference<>())	\
    .def("clearHistory",&hArray<TYPE>::clearHistory,return_internal_reference<>())	\
    .def("setHistory",&hArray<TYPE>::setHistory,return_internal_reference<>())	\
    .def("getHistory",&hArray<TYPE>::getHistory,return_internal_reference<>())	\
    .def("isTrackingHistory",&hArray<TYPE>::isTrackingHistory)	\
    .def("history",&hArray<TYPE>::printHistory)	\
    .def("setKey",&hArray<TYPE>::setKey,return_internal_reference<>())	\
    .def("getKey",&hArray<TYPE>::getKey)                                   \
    .def("writeRaw",&hArray<TYPE>::writeRaw)                               \
    .def("readRaw",&hArray<TYPE>::readRaw)
;

template <class T> HString pretty_vec(hArray<T> & a, const HInteger maxlen);
void hArray_trackHistory(HBool on);

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
//#include "mArray.def.h"

#undef HFPP_FILETYPE


#endif /* CR_PIPELINE_ARRAY_H */
