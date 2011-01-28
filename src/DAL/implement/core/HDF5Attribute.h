/***************************************************************************
 *   Copyright (C) 2011                                                    *
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

#ifndef HDF5ATTRIBUTE_H
#define HDF5ATTRIBUTE_H

// Standard library header files
#include <iostream>
#include <string>

// DAL header files
#include "HDF5Object.h"

namespace DAL { // Namespace DAL -- begin
  
  /*!
    \class HDF5Attribute
    
    \ingroup DAL
    \ingroup core
    
    \brief Brief description for class HDF5Attribute
    
    \author Lars B&auml;hren

    \date 2011/01/27

    \test tHDF5Attribute.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class HDF5Attribute : public HDF5Object {

    //! Datatype identifier
    hid_t itsDatatype;
    //! Dataspace identifier
    hid_t itsDataspace;
    //! Rank of the dataspace
    int   itsRank;
    //! Error status
    herr_t itsStatus;

  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    HDF5Attribute ();
    
    //! Copy constructor
    HDF5Attribute (HDF5Attribute const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~HDF5Attribute ();
    
    // === Operators ============================================================
    
    //! Overloading of the copy operator
    HDF5Attribute& operator= (HDF5Attribute const &other); 
    
    // === Parameter access =====================================================
    
    //! Get the datatype identifier
    inline hid_t datatype () const {
      return itsDatatype;
    }

    //! Get the dataspace identifier
    inline hid_t dataspace () const {
      return itsDataspace;
    }

    //! Get the rank of the dataspace
    inline int rank () const {
      return itsRank;
    }

    //! Get error status of the object
    inline herr_t status () const {
      return itsStatus;
    }

    /*!
      \brief Get the name of the class
      \return className -- The name of the class, HDF5Attribute.
    */
    inline std::string className () const {
      return "HDF5Attribute";
    }

    //! Provide a summary of the object's internal parameters and status
    inline void summary () {
      summary (std::cout);
    }

    //! Provide a summary of the object's internal parameters and status
    void summary (std::ostream &os);    

    // === Public methods =======================================================

    bool open (hid_t const &location,
	       std::string const &name,
	       hid_t const &access=H5P_DEFAULT);

    //! Returns the datatype class identifier.
    inline H5T_class_t datatypeClass () {
      return HDF5Object::datatypeClass(itsLocation);
    }
    
    // === Static methods =======================================================
    
    /*!
      \brief Set attribute
      \param location -- HDF5 identifier for the object to which the attribute
             is attached.
      \param name    -- Name of the attribute.
      \param data    -- Data value(s) to be assigned to the attribute
      \param size    -- nof. element in the data array.
      \return status -- Status of the operation
    */
    template <class T>
      static bool setAttribute (hid_t const &location,
				std::string const &name,
				T const *data,
				unsigned int const &size,
				hid_t const &datatype)
      {
	bool status       = true;
	hid_t   attribute = 0;
	hid_t   dataspace = 0;
	hsize_t dims[1]   = { size };
	hsize_t *maxdims  = 0;
	herr_t h5err      = 0;

	/*____________________________________________________________
	  Check if attribute 'name' already exits at the given
	  'location'; if this is not the case, we need to create the 
	  attribute.
	*/

	switch (HDF5Object::objectType(location,name)) {
	case H5I_ATTR:
	  attribute = H5Aopen (location,
			       name.c_str(),
			       H5P_DEFAULT);
	  break;
	case H5I_BADID:
	  {
	    /* Create dataspace for the attribute */
	    dataspace = H5Screate_simple (1, dims, maxdims );
	    if (H5Iis_valid(dataspace)) {
	      /* Create the attribute itself ... */
	      attribute = H5Acreate (location,
				     name.c_str(),
				     datatype,
				     dataspace,
				     0,
				     0);
	      /* ... and check if creation was successful */
	      if (H5Iis_valid(attribute)) {
		status = true;
	      } else {
		std::cerr << "[HDF5Attribute::setAttribute]"
			  << " H5Acreate() failed to create attribute "
			  << name
			  << std::endl;
		status = false;
	      }
	    } else {
	      std::cerr << "[HDF5Attribute::setAttribute]"
			<< " H5Screate_simple() failed to create dataspace!"
			<< std::endl;
	      status = false;
	    }
	  }
	  break;
	default:
	  status = false;
	  break;
	};

	/*____________________________________________________________
	  H5Awrite() returns a non-negative value if successful;
	  otherwise returns a negative value. 
	  Datatype conversion takes place at the time of a read or
	  write and is automatic. See the Data Conversion section of
	  "The Data Type Interface (H5T)" in the "HDF5 User's Guide"
	  for a discussion of data conversion, including the range of
	  conversions currently supported by the HDF5 libraries. 
	*/
	
	if (status) {
	  /* Write the data to the attribute ... */
	  h5err = H5Awrite (attribute, datatype, data);
	  /* ... and check the return value of the operation */
	  if (h5err<0) {
	    std::cerr << "[HDF5Attribute::setAttribute]"
		      << " H5Awrite() failed to write attribute!"
		      << std::endl;
	    status = false;
	  }
	}

	/*____________________________________________________________
	  Release HDF5 object handles
	*/
	HDF5Object::close (dataspace);
	HDF5Object::close (attribute);
	
	return status;
      }
    
    /*!
      \brief Set attribute
      \param location -- HDF5 identifier for the object to which the attribute
             is attached.
      \param name    -- Name of the attribute.
      \param data    -- Data value(s) to be assigned to the attribute
      \param size    -- nof. element in the data array.
      \return status -- Status of the operation
    */
    template <class T>
      static bool setAttribute (hid_t const &location,
				std::string const &name,
				T const *data,
				unsigned int const &size);
    
    /*!
      \brief Set attribute
      \param location -- HDF5 identifier for the object to which the attribute
             is attached.
      \param name    -- Name of the attribute.
      \param data    -- Data value(s) to be assigned to the attribute
      \return status -- Status of the operation
    */
    template <class T>
      static bool setAttribute (hid_t const &location,
			 std::string const &name,
			 std::vector<T> const &data)
      {
	return setAttribute (location, name, &data[0], data.size());
      }
    
    /*!
      \brief Set attribute
      \param location -- HDF5 identifier for the object to which the attribute
             is attached.
      \param name    -- Name of the attribute.
      \param data    -- Data value(s) to be assigned to the attribute
      \return status -- Status of the operation
    */
    template <class T>
      static bool setAttribute (hid_t const &location,
				std::string const &name,
				T const &data)
      {
	return setAttribute (location, name, &data, 1);
      }
    
  private:
    
    //! Initialize internal parameters to default values
    void init ();

    //! Unconditional copying
    void copy (HDF5Attribute const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class HDF5Attribute -- end
  
} // Namespace DAL -- end

#endif /* HDF5ATTRIBUTE_H */
  
