/* $Id: plc++demos.h 7007 2006-08-18 07:08:16Z arjenmarkus $

	All the common includes and constants needed by the C++ demo programs.
*/

#ifndef __PLCXXDEMOS_H__
#define __PLCXXDEMOS_H__

/* common includes */
#include "plstream.h"
#include <iostream>
#include <cmath>

/* common defines */
#ifndef M_PI
#define M_PI 3.1415926535897932384
#endif

/* various utility macros */
#ifndef ROUND
#define ROUND(a) (PLINT)((a)<0. ? ((a)-0.5) : ((a)+0.5))
#endif

#endif	/* __PLCXXDEMOS_H__ */
