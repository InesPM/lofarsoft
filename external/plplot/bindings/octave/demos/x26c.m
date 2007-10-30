## -*- coding: utf-8; -*-
##   
##  $Id: x26c.m 7204 2006-11-27 17:13:21Z andrewross $
##
##  Multi-lingual version of the first page of example 4.
##
##  Copyright (C) 2006 Alan Irwin
##  Copyright (C) 2006 Andrew Ross
##
##  Thanks to the following for providing translated strings for this example:
##  Valery Pipin (Russian)
## 
##  This file is part of PLplot.
##   
##  PLplot is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Library Public License as published
##  by the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##  
##  PLplot is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU Library General Public License for more details.
##   
##  You should have received a copy of the GNU Library General Public License
##  along with PLplot; if not, write to the Free Software
##  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
##


##
## This example designed just for the psttf-related devices(i.e., -dev psttfc 
## and -dev psttf).  No other devices will work.  These devices
## use liblasi (version 1.0.5 or later!) to render the fonts.  liblasi uses
## libpango to deal with CTL issues, and libfontconfig to deal with font 
## selection. Because of libfontconfig, the best choice of glyph is 
## automatically rendered for each unicode character in this example. 
## Of course, you must have the appropriate TrueType fonts installed to have 
## access to all the glyphs.
##
## Translation instructions: The strings to be translated are given by
## x_label, y_label, alty_label, title_label, and line_label below.  The 
## encoding used must be UTF-8.
##
## The following strings to be translated involve some scientific/mathematical 
## jargon which is now discussed further to help translators.
##
## (1) dB is a decibel unit, see http://en.wikipedia.org/wiki/Decibel .
## (2) degrees is an angular measure, see 
##     http://en.wikipedia.org/wiki/Degree_(angle) .
## (3) low-pass filter is one that transmits (passes) low frequencies.
## (4) pole is in the mathematical sense, see
##     http://en.wikipedia.org/wiki/Pole_(complex_analysis) .  "Single Pole"
##     means a particular mathematical transformation of the filter function has
##     a single pole, see
##     http://ccrma.stanford.edu/~jos/filters/Pole_Zero_Analysis_I.html .  
##     Furthermore, a single-pole filter must have an inverse square decline 
##     (or -20 db/decade). Since the filter plotted here does have that 
##     characteristic, it must by definition be a single-pole filter, see also
##     http://www-k.ext.ti.com/SRVS/Data/ti/KnowledgeBases/analog/document/faqs/1p.htm
## (5) decade represents a factor of 10, see
##     http://en.wikipedia.org/wiki/Decade_(log_scale) .
##

1;

function ix26c

x_label = [
  "Frequency";
  "Частота";
];

y_label = [
  "Amplitude (dB)";
  "Амплитуда (dB)";
];

alty_label = [
  "Phase shift (degrees)";
  "Фазовый сдвиг (градусы)";
];

title_label = [
  "Single Pole Low-Pass Filter";
  "Однополюсный Низко-Частотный Фильтр";
];

line_label = [
  "-20 dB/decade";
  "-20 dB/десяток";
];

## Illustration of logarithmic axes, and redefinition of window.

## Parse and process command line arguments

##    (void) plparseopts(&argc, argv, PL_PARSE_FULL);

## Initialize plplot

  plinit;
  plfont(2);

  ## Make log plots using two different styles.

  for i = 1:size(x_label)(1);
    plot1(0, deblank(x_label(i,:)), deblank(y_label(i,:)), deblank(alty_label(i,:)), deblank(title_label(i,:)), deblank(line_label(i,:)));
  endfor
    
  plend1;
end

## Log-linear plot.

function plot1(plottype, x_label, y_label, alty_label, title_label, line_label)

  pladv(0);

  ## Set up data for log plot */

  f0 = 1.0;
  i=0:100;
  freql = -2.0 + i / 20.0;
  freq = 10.0 .^ freql;
  ampl = 20.0 * log10(1.0 ./ sqrt(1.0 + (freq ./ f0).^ 2.));
  phase = -(180.0 / 3.141592654) * atan(freq ./ f0);

  plvpor(0.15, 0.85, 0.1, 0.9);
  plwind(-2.0, 3.0, -80.0, 0.0);

  ## Try different axis and labelling styles. */

  plcol0(1);
  switch (plottype)

    case 0
      plbox("bclnst", 0.0, 0, "bnstv", 0.0, 0);

    case 1
      plbox("bcfghlnst", 0.0, 0, "bcghnstv", 0.0, 0);

  endswitch

  ## Plot ampl vs freq */

  plcol0(2);
  plline(freql', ampl');
  plcol0(1);
  plptex(1.6, -30.0, 1.0, -20.0, 0.5, line_label);

  ## Put labels on */

  plcol0(1);
  plmtex("b", 3.2, 0.5, 0.5, x_label);
  plmtex("t", 2.0, 0.5, 0.5, title_label);
  plcol0(2);
  plmtex("l", 5.0, 0.5, 0.5, y_label);

  ## For the gridless case, put phase vs freq on same plot */

  if (plottype == 0) 
    plcol0(1);
    plwind(-2.0, 3.0, -100.0, 0.0);
    plbox("", 0.0, 0, "cmstv", 30.0, 3);
    plcol0(3);
    plline(freql', phase');
    plcol0(3);
    plmtex("r", 5.0, 0.5, 0.5, alty_label);
  endif

end

ix26c