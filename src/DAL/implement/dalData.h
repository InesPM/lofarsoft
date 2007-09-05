/*-------------------------------------------------------------------------*
 | $Id:: dalData.h 660 2007-08-30 16:55:31Z masters                     $ |
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

/*!
  \file dalData.h
  
  \ingroup DAL

  \author Joseph Masters

  \date 09-05-07
*/

#ifndef DALDATA_H
#define DALDATA_H

#ifndef DAL_H
#include "dal.h"
#endif


/*!
The dalData object represents an n-dimensional array of data regardless of
the underlying format.  In the case of CASA measurement sets, the data
is stored in row-major (FORTRAN) order.  For HDF5, and probably FITS, the
data is stored in column-major (C) order.  The dalData object abstracts this
away from the user and developer.

There will also be a way for the developer to get access to the c-array,
exactly as it is stored.
*/
class dalData{
	void * data;

public:

};
#endif
