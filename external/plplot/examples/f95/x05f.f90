!      $Id: x05f.f90 6572 2006-05-16 20:24:14Z airwin $
!      Draws a histogram from sample data
!
!      Copyright (C) 2004  Alan W. Irwin
!
!      This file is part of PLplot.
!
!      PLplot is free software; you can redistribute it and/or modify
!      it under the terms of the GNU General Library Public License as
!      published by the Free Software Foundation; either version 2 of the
!      License, or (at your option) any later version.
!
!      PLplot is distributed in the hope that it will be useful,
!      but WITHOUT ANY WARRANTY; without even the implied warranty of
!      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!      GNU Library General Public License for more details.
!
!      You should have received a copy of the GNU Library General Public
!      License along with PLplot; if not, write to the Free Software
!      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

      use plplot, PI => PL_PI
      implicit none

      integer i
      integer NPTS
      parameter ( NPTS = 2047 )
      real(kind=plflt) data(NPTS), delta
!      Process command-line arguments
      call plparseopts(PL_PARSE_FULL)

!      Initialize plplot
      call plinit()
!      Fill up data points

      delta = 2.0_plflt * PI / dble (NPTS)
      do i=1,NPTS
        data(i)=sin(delta*dble (i-1))
      enddo

      call plcol0(1)
      call plhist(data, -1.1_plflt, 1.1_plflt, 44, 0)
      call plcol0(2)
      call pllab('#frValue','#frFrequency', &
        '#frPLplot Example 5 - Probability function of Oscillator')

      call plend
      end
