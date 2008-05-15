!      $Id: x23f.f90 6572 2006-05-16 20:24:14Z airwin $
!
!   Displays Greek letters and mathematically interesting Unicode ranges
!
!   Copyright (C) 2005 Alan Irwin
!   Copyright (C) 2005 Andrew Ross
!
!
!   This file is part of PLplot.
!
!   PLplot is free software; you can redistribute it and/or modify
!   it under the terms of the GNU General Library Public License as published
!   by the Free Software Foundation; either version 2 of the License, or
!   (at your option) any later version.
!
!   PLplot is distributed in the hope that it will be useful,
!   but WITHOUT ANY WARRANTY; without even the implied warranty of
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!   GNU Library General Public License for more details.
!
!   You should have received a copy of the GNU Library General Public License
!   along with PLplot; if not, write to the Free Software
!   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
!
!

      program x23f
      use plplot
      implicit none

      real(kind=plflt) xmin, xmax, ymin, ymax, ycharacter_scale, yoffset
      real(kind=plflt) chardef, charht, deltax, deltay, x, y
      integer i, j, page, length, slice
      character*20 cmdString

!
!  Displays Greek letters and mathematically interesting Unicode ranges
!
      character*5  greek(48)
      integer      type1(166)
      character*80 title(11)
      integer      lo(11)
      integer      hi(11)
      integer      nxcells(11)
      integer      nycells(11)
      integer      offset(11)

      data (greek(i) ,i=1,48) / &
       '#gA','#gB','#gG','#gD','#gE','#gZ', &
       '#gY','#gH','#gI','#gK','#gL','#gM', &
       '#gN','#gC','#gO','#gP','#gR','#gS', &
       '#gT','#gU','#gF','#gX','#gQ','#gW', &
       '#ga','#gb','#gg','#gd','#ge','#gz', &
       '#gy','#gh','#gi','#gk','#gl','#gm', &
       '#gn','#gc','#go','#gp','#gr','#gs', &
       '#gt','#gu','#gf','#gx','#gq','#gw'  /

      data (type1(i) ,i=1,75) / &
       z'0020',z'0021',z'0023',z'0025',z'0026', &
       z'0028',z'0029',z'002b',z'002c',z'002e', &
       z'002f',z'0030',z'0031',z'0032',z'0033', &
       z'0034',z'0035',z'0036',z'0037',z'0038', &
       z'0039',z'003a',z'003b',z'003c',z'003d', &
       z'003e',z'003f',z'005b',z'005d',z'005f', &
       z'007b',z'007c',z'007d',z'00a9',z'00ac', &
       z'00ae',z'00b0',z'00b1',z'00d7',z'00f7', &
       z'0192',z'0391',z'0392',z'0393',z'0394', &
       z'0395',z'0396',z'0397',z'0398',z'0399', &
       z'039a',z'039b',z'039c',z'039d',z'039e', &
       z'039f',z'03a0',z'03a1',z'03a3',z'03a4', &
       z'03a5',z'03a6',z'03a7',z'03a8',z'03a9', &
       z'03b1',z'03b2',z'03b3',z'03b4',z'03b5', &
       z'03b6',z'03b7',z'03b8',z'03b9',z'03ba' /
      data (type1(i) ,i=76,166) / &
       z'03bb',z'03bc',z'03bd',z'03be',z'03bf', &
       z'03c0',z'03c1',z'03c2',z'03c3',z'03c4', &
       z'03c5',z'03c6',z'03c7',z'03c8',z'03c9', &
       z'03d1',z'03d2',z'03d5',z'03d6',z'2022', &
       z'2026',z'2032',z'2033',z'203e',z'2044', &
       z'2111',z'2118',z'211c',z'2122',z'2126', &
       z'2135',z'2190',z'2191',z'2192',z'2193', &
       z'2194',z'21b5',z'21d0',z'21d1',z'21d2', &
       z'21d3',z'21d4',z'2200',z'2202',z'2203', &
       z'2205',z'2206',z'2207',z'2208',z'2209', &
       z'220b',z'220f',z'2211',z'2212',z'2215', &
       z'2217',z'221a',z'221d',z'221e',z'2220', &
       z'2227',z'2228',z'2229',z'222a',z'222b', &
       z'2234',z'223c',z'2245',z'2248',z'2260', &
       z'2261',z'2264',z'2265',z'2282',z'2283', &
       z'2284',z'2286',z'2287',z'2295',z'2297', &
       z'22a5',z'22c5',z'2320',z'2321',z'2329', &
       z'232a',z'25ca',z'2660',z'2663',z'2665', &
       z'2666' /

      data (title(i) ,i=1,11 )/ &
   "#<0x10>PLplot Example 23 - Greek Letters", &
   "#<0x10>PLplot Example 23 - Type 1 Symbol Font Glyphs by Unicode (a)", &
   "#<0x10>PLplot Example 23 - Type 1 Symbol Font Glyphs by Unicode (b)", &
   "#<0x10>PLplot Example 23 - Type 1 Symbol Font Glyphs by Unicode (c)", &
   "#<0x10>PLplot Example 23 - Number Forms Unicode Block", &
   "#<0x10>PLplot Example 23 - Arrows Unicode Block (a)", &
   "#<0x10>PLplot Example 23 - Arrows Unicode Block (b)", &
   "#<0x10>PLplot Example 23 - Mathematical Operators Unicode Block (a)", &
   "#<0x10>PLplot Example 23 - Mathematical Operators Unicode Block (b)", &
   "#<0x10>PLplot Example 23 - Mathematical Operators Unicode Block (c)", &
   "#<0x10>PLplot Example 23 - Mathematical Operators Unicode Block (d)" /

      data (lo(i) ,i=1,11) / &
        z'0', &
        z'0', &
        z'40', &
        z'80', &
        z'2153', &
        z'2190', &
        z'21d0', &
        z'2200', &
        z'2240', &
        z'2280', &
        z'22c0' /

      data (hi(i) ,i=1,11) / &
        z'30', &
        z'40', &
        z'80', &
        z'A6', &
        z'2184', &
        z'21d0', &
        z'2200', &
        z'2240', &
        z'2280', &
        z'22c0', &
        z'2300' /

      data (nxcells(i) ,i=1,11) / &
      12, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8 /

      data (nycells(i) ,i=1,11) / &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8, &
      8 /

!  non-zero values Must be consistent with nxcells and nycells. */
      data (offset(i) ,i=1,11) / &
      0, &
      0, &
      64, &
      128, &
      0, &
      0, &
      0, &
      0, &
      0, &
      0, &
      0 /

      call plparseopts(PL_PARSE_FULL)

      call plinit()

      do 130 page = 1,11
         call pladv(0)

!        Set up viewport and window

         call plvpor(0.02_plflt, 0.98_plflt, 0.02_plflt, 0.90_plflt)
         call plwind(0.0_plflt, 1.0_plflt, 0.0_plflt, 1.0_plflt)
         call plgspa(xmin, xmax, ymin, ymax)
         call plschr(0._plflt, 0.8_plflt)
         ycharacter_scale = (1.0_plflt - 0.0_plflt)/(ymax-ymin)

!        Factor should be 0.5, but heuristically it turns out to be larger.

         call plgchr(chardef, charht)
         yoffset = 1.0_plflt*charht*ycharacter_scale

!        Draw the grid using plbox

         call plcol0(2)
         deltax = 1.0_plflt/nxcells(page)
         deltay = 1.0_plflt/nycells(page)
         call plbox("bcg", deltax, 0, "bcg", deltay, 0)
         call plcol0(15)
         length=hi(page)-lo(page)
         slice = 1
         do 120 j=nycells(page),0,-1
            y = (j-0.5_plflt)*deltay
            do 110 i=1,nxcells(page)
               x  = (i-0.5_plflt)*deltax
               if (slice .le. length) then
                  if (page .eq. 1) then
                     write(cmdString, '("#",a)') Greek(slice)
                  elseif (page .ge. 1 .and. page .le. 4) then
                     write(cmdString, '("##[0x",z4.4,"]")') &
                        type1(offset(page)+slice)
!                     Not required for command which is case insensitive,
!                     but lowercase the command simply to get same
!                     lowercase display of command as corresponding C example.
                     call lowercase23(cmdString)
                  elseif (page .ge. 5) then
                     write(cmdString, '("##[0x",z4.4,"]")') &
                        lo(page)+slice-1
!                     Not required for command which is case insensitive,
!                     but lowercase the command simply to get same
!                     lowercase display of command as corresponding C example.
                     call lowercase23(cmdString)
                  endif
                  call plptex(x,y+yoffset,1._plflt,0._plflt,0.5_plflt, &
                    cmdString(2:20))
                  call plptex(x,y-yoffset,1._plflt,0._plflt,0.5_plflt, &
                    cmdString)
               endif
               slice = slice + 1
  110       continue

  120    continue

         call plschr(0._plflt, 1.0_plflt)
         call plmtex("t", 1.5_plflt, 0.5_plflt, 0.5_plflt, title(page))
  130 continue

!     Restore defaults

      call plcol0(1)

      call plend()

      end

      subroutine lowercase23(string)
      implicit none
      character*(*) string
      integer i, len, iascii
      do i = 1, len(string)
        iascii = iachar(string(i:i))
        if(65.le.iascii.and.iascii.le.90) then
!          convert uppercase to lowercase.
          string(i:i) = achar(iascii+32)
        endif
      enddo
      end
