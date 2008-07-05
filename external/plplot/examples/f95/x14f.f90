!      $Id: x14f.f90 6883 2006-07-26 14:06:57Z andrewross $
!      Demo of multiple stream/window capability
!
!      Copyright (C) 2004  Arjen Markus
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

! Plots several simple functions from other example programs.
!
! This version sends the output of the first 4 plots (one page) to two
! independent streams.

      use plplot
      implicit none

      integer i, digmax
      integer xleng0 , yleng0 , xoff0 , yoff0
      integer xleng1 , yleng1 , xoff1 , yoff1

      character*80 driver
      character*15 geometry_master
      character*15 geometry_slave

      real(kind=plflt)  x(101), y(101)
      real(kind=plflt)  xs(6), ys(6)
      real(kind=plflt)  xscale, yscale, xoff, yoff
      common /plotdat/ x, y, xs, ys, xscale, yscale, xoff, yoff
      character*80 version

      xleng0 = 400
      yleng0 = 300
      xoff0  = 200
      yoff0  = 200
      xleng1 = 400
      xleng1 = 300
      xoff1  = 500
      yoff1  = 500

      geometry_master = '500x410+100+200'
      geometry_slave = '500x410+650+200'

!      Process command-line arguments
      call plparseopts(PL_PARSE_FULL)

      call plgdev(driver)

      write(*,*) 'Demo of multiple output streams via the ', &
        trim(driver), ' driver.'
      write(*,*) 'Running with the second stream as slave ', &
        'to the first.'
      write(*,*)

!      Set up first stream

      call plsetopt( 'geometry', geometry_master)

      call plsdev(driver)
      call plssub(2, 2)
      call plinit()

!      Start next stream

      call plsstrm(1)

!      Turn off pause to make this a slave (must follow master)

      call plsetopt( 'geometry', geometry_slave)
      call plspause(.false.)
      call plsdev(driver)
      call plinit()

!      Set up the data & plot
!      Original case

      call plsstrm(0)

      xscale = 6._plflt
      yscale = 1._plflt
      xoff = 0._plflt
      yoff = 0._plflt
      call plot1()

!      Set up the data & plot

      xscale = 1._plflt
      yscale = 1.d+6
      call plot1()

!      Set up the data & plot

      xscale = 1._plflt
      yscale = 1.d-6
      digmax = 2
      call plsyax(digmax, 0)
      call plot1()

!      Set up the data & plot

      xscale = 1._plflt
      yscale = 0.0014_plflt
      yoff = 0.0185_plflt
      digmax = 5
      call plsyax(digmax, 0)
      call plot1()

!      To slave
!      The pleop() ensures the eop indicator gets lit.

      call plsstrm(1)
      call plot4()
      call pleop()

!      Back to master

      call plsstrm(0)
      call plot2()
      call plot3()

!      To slave

      call plsstrm(1)
      call plot5()
      call pleop()

!      Back to master to wait for user to advance

      call plsstrm(0)
      call pleop()

!      Call plend to finish off.

      call plend()
      end

!======================================================================

      subroutine plot1()
      use plplot
      implicit none

      real(kind=plflt) x(101), y(101)
      real(kind=plflt) xs(6), ys(6)
      real(kind=plflt)  xscale, yscale, xoff, yoff, &
        xmin, xmax, ymin, ymax
      integer i
      common /plotdat/ x, y, xs, ys, xscale, yscale, xoff, yoff

      do i = 1, 60
        x(i) = xoff + xscale * dble(i)/60.0_plflt
        y(i) = yoff + yscale * x(i)**2
      enddo

      xmin = x(1)
      xmax = x(60)
      ymin = y(1)
      ymax = y(60)

      do i = 1, 6
        xs(i) = x((i-1)*10+4)
        ys(i) = y((i-1)*10+4)
      enddo

!      Set up the viewport and window using PLENV. The range in X is
!      0.0 to 6.0, and the range in Y is 0.0 to 30.0. The axes are
!      scaled separately (just = 0), and we just draw a labelled
!      box (axis = 0).

      call plcol0(1)
      call plenv( xmin, xmax, ymin, ymax, 0, 0 )
      call plcol0(6)
      call pllab( '(x)', '(y)', '#frPLplot Example 1 - y=x#u2' )

!      Plot the data points

      call plcol0(9)
      call plpoin(xs, ys, 9)

!      Draw the line through the data

      call plcol0(4)
      call plline(x(:60), y(:60))
      call plflush
      end

!======================================================================

      subroutine plot2()
      use plplot
      implicit none
      real(kind=plflt)  x(101), y(101)
      real(kind=plflt)  xs(6), ys(6)
      real(kind=plflt)  xscale, yscale, xoff, yoff
      integer i
      common /plotdat/ x, y, xs, ys, xscale, yscale, xoff, yoff

!======================================================================
!
!      Set up the viewport and window using PLENV. The range in X is
!      -2.0 to 10.0, and the range in Y is -0.4 to 2.0. The axes are
!      scaled separately (just = 0), and we draw a box with axes
!      (axis = 1).

      call plcol0(1)
      call plenv(-2.0_plflt, 10.0_plflt, -0.4_plflt, 1.2_plflt, 0, 1 )
      call plcol0(2)
      call pllab( '(x)', 'sin(x)/x', &
                  '#frPLplot Example 1 - Sinc Function' )

!      Fill up the arrays

      do i = 1, 100
        x(i) = (i-20.0_plflt)/6.0_plflt
        y(i) = 1.0_plflt
        if (x(i) .ne. 0.0_plflt) y(i) = sin(x(i)) / x(i)
      enddo
!      Draw the line

      call plcol0(3)
      call plline(x(:100), y(:100))
      call plflush
      end

!======================================================================

      subroutine plot3()
!
!      For the final graph we wish to override the default tick intervals,
!      and so do not use_ PLENV

      use plplot, PI => PL_PI
      implicit none
      real(kind=plflt)  x(101), y(101)
      real(kind=plflt)  xs(6), ys(6)
      real(kind=plflt)  xscale, yscale, xoff, yoff
      integer i
      common /plotdat/ x, y, xs, ys, xscale, yscale, xoff, yoff
      call pladv(0)

!      Use_ standard viewport, and define X range from 0 to 360 degrees,
!      Y range from -1.2 to 1.2.

      call plvsta()
      call plwind( 0.0_plflt, 360.0_plflt, -1.2_plflt, 1.2_plflt )

!      Draw a box with ticks spaced 60 degrees apart in X, and 0.2 in Y.

      call plcol0(1)
      call plbox( 'bcnst', 60.0_plflt, 2, 'bcnstv', 0.2_plflt, 2 )

!      Superimpose a dashed line grid, with 1.5 mm marks and spaces. With
!      only a single mark and space element, we do not need arrays

      call plstyl( 1, 1500, 1500 )
      call plcol0(2)
      call plbox( 'g', 30.0_plflt, 0, 'g', 0.2_plflt, 0 )
      call plstyl( 0, 0, 0 )

      call plcol0(3)
      call pllab( 'Angle (degrees)', 'sine', &
                  '#frPLplot Example 1 - Sine function' )

      do i = 1, 101
        x(i) = 3.6_plflt * (i-1)
        y(i) = sin( x(i) * PI/180.0_plflt )
      enddo

      call plcol0(4)
      call plline(x, y)
      call plflush
      end

!======================================================================

      subroutine plot4()

      use plplot, PI => PL_PI
      implicit none
      character*3 text
      real(kind=plflt) x0(0:360), y0(0:360)
      real(kind=plflt) x(0:360), y(0:360), dtr, theta, dx, dy, r
      integer i, j, nsp

      dtr = PI/180.0_plflt
      do i=0,360
        x0(i) = cos(dtr * dble (i))
        y0(i) = sin(dtr * dble (i))
      enddo

!      Set up viewport and window, but do not draw box

      call plenv(-1.3_plflt, 1.3_plflt, -1.3_plflt, 1.3_plflt, 1, -2)
      do i = 1,10
        do j = 0,360
          x(j) = 0.1_plflt*i*x0(j)
          y(j) = 0.1_plflt*i*y0(j)
        enddo

!        Draw circles for polar grid

        call plline(x,y)
      enddo
      call plcol0(2)
      do i = 0,11
        theta = 30.0_plflt*i
        dx = cos(dtr*theta)
        dy = sin(dtr*theta)

!        Draw radial spokes for polar grid

        call pljoin(0.0_plflt, 0.0_plflt, dx, dy)
        write (text,'(i3)') nint(theta)

!        Write labels for angle

        text = text(nsp(text):)
!        Slightly off zero to avoid floating point logic flips at
!        90 and 270 deg.
        if (dx.ge.-0.00001_plflt) then
          call plptex(dx, dy, dx, dy, -0.15_plflt, text)
        else
          call plptex(dx, dy, -dx, -dy, 1.15_plflt, text)
        end if
      enddo
!      Draw the graph

      do i=0,360
        r = sin(dtr*dble (5*i))
        x(i) = x0(i) * r
        y(i) = y0(i) * r
      enddo
      call plcol0(3)
      call plline(x,y)

      call plcol0(4)
      call plmtex('t', 2.0_plflt, 0.5_plflt, 0.5_plflt, &
        '#frPLplot Example 3 - r(#gh)=sin 5#gh')

!      Flush the plot at end

      call plflush
      end

!======================================================================

      integer function nsp(text)
!      ==================

!      Find first non-space character
      use plplot
      implicit none

      character*(*) text
      integer l, len

      l = len(text)
      nsp = 1
      do while(text(nsp:nsp).eq.' ' .and. nsp.lt.l)
        nsp = nsp+1
      enddo
      end

!======================================================================

      subroutine plot5()

      use plplot, PI => PL_PI
      implicit none
      integer i, j, nptsx, nptsy, xdim, ydim
!      xdim and ydim are the absolute static dimensions.
!      nptsx, and nptsy are the (potentially dynamic) defined area of the 2D
!      arrays that is actually used.
      parameter (xdim=99, ydim=100, nptsx=35,nptsy=46)

      real(kind=plflt) z(xdim, ydim), w(xdim, ydim), clevel(11), &
        xg1(xdim), yg1(ydim), &
        xg2(xdim, ydim), yg2(xdim, ydim)
      real(kind=plflt) xx, yy, argx, argy, distort
      real(kind=plflt) tr(6)

      data clevel /-1._plflt, -0.8_plflt, -0.6_plflt, -0.4_plflt, &
        -0.2_plflt, &
        0._plflt, 0.2_plflt, 0.4_plflt, 0.6_plflt ,0.8_plflt, 1._plflt/

      tr(1) = 2._plflt/dble(nptsx-1)
      tr(2) = 0.0_plflt
      tr(3) = -1.0_plflt
      tr(4) = 0.0_plflt
      tr(5) = 2._plflt/dble(nptsy-1)
      tr(6) = -1.0_plflt

!      Calculate the data matrices.
      do i=1,nptsx
        xx = dble(i-1-(nptsx/2))/dble (nptsx/2)
        do j=1,nptsy
          yy = dble(j-1-(nptsy/2))/dble (nptsy/2) - 1.0_plflt
          z(i,j) = xx*xx - yy*yy
          w(i,j) = 2._plflt*xx*yy
        enddo
      enddo

!      Build the 1-d coord arrays.
      distort = 0.4_plflt
      do i=1,nptsx
        xx = -1._plflt + dble(i-1)*2._plflt/dble(nptsx-1)
        xg1(i) = xx + distort*cos(0.5_plflt*PI*xx)
      enddo

      do j=1,nptsy
        yy = -1._plflt + dble(j-1)*2._plflt/dble(nptsy-1)
        yg1(j) = yy - distort*cos(0.5_plflt*PI*yy)
      enddo

!      Build the 2-d coord arrays.
      do i=1,nptsx
        xx = -1._plflt + dble(i-1)*2._plflt/dble(nptsx-1)
        argx = 0.5_plflt*PI*xx
        do j=1,nptsy
          yy = -1._plflt + dble(j-1)*2._plflt/dble(nptsy-1)
          argy = 0.5_plflt*PI*yy
          xg2(i,j) = xx + distort*cos(argx)*cos(argy)
          yg2(i,j) = yy - distort*cos(argx)*cos(argy)
        enddo
      enddo

!      Plot using identity transform
      call plenv(-1.0_plflt, 1.0_plflt, -1.0_plflt, 1.0_plflt, 0, 0)
      call plcol0(2)
      call plcont(z,1,nptsx,1,nptsy,clevel,tr)
      call plstyl(1,1500,1500)
      call plcol0(3)
      call plcont(w,1,nptsx,1,nptsy,clevel,tr)
      call plstyl(0,1500,1500)
      call plcol0(1)
      call pllab('X Coordinate', 'Y Coordinate', &
        'Streamlines of flow')

      call plflush
      end
