/*============================================================================
*
*   WCSLIB 4.2 - an implementation of the FITS WCS convention.
*   Copyright (C) 1995-2005, Mark Calabretta
*
*   WCSLIB is free software; you can redistribute it and/or modify it under
*   the terms of the GNU General Public License as published by the Free
*   Software Foundation; either version 2 of the License, or (at your option)
*   any later version.
*
*   WCSLIB is distributed in the hope that it will be useful, but WITHOUT ANY
*   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
*   details.
*
*   You should have received a copy of the GNU General Public License along
*   with WCSLIB; if not, write to the Free Software Foundation, Inc.,
*   59 Temple Place, Suite 330, Boston, MA  02111-1307, USA
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta
*                      Australia Telescope National Facility, CSIRO
*                      PO Box 76
*                      Epping NSW 1710
*                      AUSTRALIA
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   http://www.atnf.csiro.au/~mcalabre/index.html
*   $Id: tlin.c,v 4.2 2005/09/22 02:15:15 cal103 Exp $
*=============================================================================
*
*   tlin tests the linear transformation routines supplied with WCSLIB.
*
*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include <lin.h>

int NAXIS  = 5;
int NCOORD = 2;
int NELEM  = 9;

double CRPIX[5] =  {256.0, 256.0,  64.0, 128.0,   1.0};
double PC[5][5] = {{  1.0,   0.5,   0.0,   0.0,   0.0},
                   {  0.5,   1.0,   0.0,   0.0,   0.0},
                   {  0.0,   0.0,   1.0,   0.0,   0.0},
                   {  0.0,   0.0,   0.0,   1.0,   0.0},
                   {  0.0,   0.0,   0.0,   0.0,   1.0}};
double CDELT[5] =  {  1.2,   2.3,   3.4,   4.5,   5.6};

double pix[2][9] = {{303.0, 265.0, 112.4, 144.5,  28.2, 0.0, 0.0, 0.0, 0.0},
                    { 19.0,  57.0,   2.0,  15.0,  42.0, 0.0, 0.0, 0.0, 0.0}};
double img[2][9];

int main()

{
   int i, j, k, status;
   double *pcij;
   struct linprm lin;


   printf("Testing WCSLIB linear transformation routines (tlin.c)\n"
          "------------------------------------------------------\n");

   /* List status return messages. */
   printf("\nList of lin status return values:\n");
   for (status = 1; status <= 3; status++) {
      printf("%4d: %s.\n", status, lin_errmsg[status]);
   }


   lin.flag = -1;
   linini(1, NAXIS, &lin);

   pcij = lin.pc;
   for (i = 0; i < lin.naxis; i++) {
      lin.crpix[i] = CRPIX[i];

      for (j = 0; j < lin.naxis; j++) {
         *(pcij++) = PC[i][j];
      }

      lin.cdelt[i] = CDELT[i];
   }

   for (k = 0; k < NCOORD; k++) {
      printf("\nPIX %d:", k+1);
      for (j = 0; j < NAXIS; j++) {
         printf("%14.8f", pix[k][j]);
      }
   }
   printf("\n");

   if (status = linp2x(&lin, NCOORD, NELEM, pix[0], img[0])) {
      printf("linp2x ERROR %d\n", status);
      return 1;
   }

   for (k = 0; k < NCOORD; k++) {
      printf("\nIMG %d:", k+1);
      for (j = 0; j < NAXIS; j++) {
         printf("%14.8f", img[k][j]);
      }
   }
   printf("\n");

   if (status = linx2p(&lin, NCOORD, NELEM, img[0], pix[0])) {
      printf("linx2p ERROR %d\n", status);
      return 1;
   }

   for (k = 0; k < NCOORD; k++) {
      printf("\nPIX %d:", k+1);
      for (j = 0; j < NAXIS; j++) {
         printf("%14.8f", pix[k][j]);
      }
   }
   printf("\n");

   if (status = linp2x(&lin, NCOORD, NELEM, pix[0], img[0])) {
      printf("linp2x ERROR %d\n", status);
      return 1;
   }

   for (k = 0; k < NCOORD; k++) {
      printf("\nIMG %d:", k+1);
      for (j = 0; j < NAXIS; j++) {
         printf("%14.8f", img[k][j]);
      }
   }
   printf("\n");

   linfree(&lin);

   return 0;
}
