-- $Id: x12a.adb.cmake 8022 2007-11-13 18:58:30Z airwin $

-- Bar chart demo.

-- Copyright (C) 2006 Jerry Bauck

-- This file is part of PLplot.

-- PLplot is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Library Public License as published
-- by the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.

-- PLplot is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Library General Public License for more details.

-- You should have received a copy of the GNU Library General Public License
-- along with PLplot; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

with
    Ada.Text_IO,
    Ada.Numerics,
    Ada.Numerics.Long_Elementary_Functions,
    Ada.Strings,
    Ada.Strings.Fixed,
    PLplot_Traditional,
    PLplot_Auxiliary;

use
    Ada.Text_IO,
    Ada.Numerics,
    Ada.Numerics.Long_Elementary_Functions,
    Ada.Strings,
    Ada.Strings.Fixed,
    PLplot_Traditional,
    PLplot_Auxiliary;

-- COMMENT THIS LINE IF YOUR COMPILER DOES NOT INCLUDE THESE 
-- DEFINITIONS, FOR EXAMPLE, IF IT IS NOT ADA 2005 WITH ANNEX G.3 COMPLIANCE.
--with Ada.Numerics.Long_Real_Arrays; use Ada.Numerics.Long_Real_Arrays;
@Ada_Is_2007_With_and_Use_Numerics@

------------------------------------------------------------------------------
-- Does a simple bar chart, using color fill.  If color fill is
-- unavailable, pattern fill is used instead (automatic).
------------------------------------------------------------------------------

procedure x12a is
    y0 : Real_Vector (0 .. 9);
    
    procedure plfbox (x0, y0 : Long_Float) is
        x, y : Real_Vector (0 ..3);
    begin
        x(0) := x0;
        y(0) := 0.0;

        x(1) := x0;
        y(1) := y0;

        x(2) := x0 + 1.0;
        y(2) := y0;

        x(3) := x0 + 1.0;
        y(3) := 0.0;

        plfill(x, y);
        plcol0(1);
        pllsty(1);
        plline(x, y);
    end plfbox;

begin
    -- Parse and process command line arguments.
    plparseopts(PL_PARSE_FULL); 

    -- Initialize plplot.
    plinit;

    pladv(0);
    plvsta;
    plwind(1980.0, 1990.0, 0.0, 35.0);
    plbox("bc", 1.0, 0, "bcnv", 10.0, 0);
    plcol0(2);
    pllab("Year", "Widget Sales (millions)", "#frPLplot Example 12");

    y0(0) :=  5.0;
    y0(1) := 15.0;
    y0(2) := 12.0;
    y0(3) := 24.0;
    y0(4) := 28.0;
    y0(5) := 30.0;
    y0(6) := 20.0;
    y0(7) :=  8.0;
    y0(8) := 12.0;
    y0(9) :=  3.0;

    for i in y0'range loop
        plcol0(i + 1);
        plpsty(0);
        plfbox((1980.0 + Long_Float(i)), y0(i));
        plptex(1980.0 + Long_Float(i) + 0.5, y0(i) + 1.0, 1.0, 0.0, 0.5, Trim(Integer'image(Integer(y0(i))), Left));
        plmtex("b", 1.0, (Long_Float(i) + 1.0) * 0.1 - 0.05, 0.5, Trim(Integer'image(1980 + i), Left));
    end loop;

    -- Don't forget to call plend to finish off!
    plend;
end x12a;
