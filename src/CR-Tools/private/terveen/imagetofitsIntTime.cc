
/*-------------------------------------------------------------------------*
 | $Id:: imagetofitsIntTime2.cc 															                   
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sef Welles                                                            *
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

#include <crtools.h>
#include <Utilities/TestsCommon.h>

//#include <images/Images/ImageFITSConverter.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>
#include <images/Images/RebinImage.h>
//#include <casa/Arrays/Slicer.h>
using namespace casa;
using namespace std;

int main(int argc, char *argv[])
{
  string infile;
  string datatype;
  int timecomp;
  
  /*
    Check if filename of the dataset and the format are provided on the command line.
  */
  if (argc < 2) {
    cerr << "Usage: imagetofits <input.event> <datatype> <time compression factor (default 100)>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
    return 1;
  } else {
      infile = argv[1];
      if (argc < 3) {
      datatype = "hdf5";
      timecomp = 100;
      } else {
	datatype = argv[2];
	if (argc < 4) {
	  timecomp = 100;
	} else {
	    timecomp = atoi(argv[3]);
          }
        }
    }


  
  String fitsName = "new.fits";
  String error;
  Bool ok;
  
  // Open the image and convert it to fits:
  if (datatype == "hdf5") {
	  HDF5Image<Float> imageIn (infile);
	  
	  
		// Regridding of input file:
		IPosition factors (imageIn.shape());
		factors    = 1;
		factors(4) = timecomp;
		//factors(0) = 60;
		//factors(1) = 60;
        
		RebinImage<Float> rb(imageIn, factors);
	
	/* Summary of image properties */
		cout << "-- Summary of the original image:" << std::endl;
		DAL::summary (imageIn);
		cout << "-- Summary of the image after rebinning:" << std::endl;
		DAL::summary (rb);
		
	
		//ok = CR::image2fits(rb, fitsName, true, true, true);	
	    String error;
	    ok = casa::ImageFITSConverter::ImageToFITS ( error, rb, fitsName, 500);
	  cout << error;
		if(ok){
		cout<<"Image converted to "<<fitsName<<"."<<endl;
	  }		
  } else if (datatype == "paged") {
    PagedImage<Float> imageIn (infile);
	// Regridding of input file:
		IPosition factors (imageIn.shape());
		factors    = 1;
		factors(4) = timecomp;
		RebinImage<Float> rb(imageIn, factors);

	/* Summary of image properties */
		cout << "-- Summary of the original image:" << std::endl;
		DAL::summary (imageIn);
		cout << "-- Summary of the image after rebinning:" << std::endl;
		DAL::summary (rb);


		ok = CR::image2fits(rb, fitsName);	
	
		if(ok){
		cout<<"Image converted to "<<fitsName<<"."<<endl;
	  }		
  } else {
    cerr << "Usage: imagetofits <input.event> <datatype> <frequency compression factor>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
  }
 
  return 0;
}
