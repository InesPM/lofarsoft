/* $Id: wxwidgets.cpp 8222 2008-02-07 13:55:45Z smekal $

   Copyright (C) 2005  Werner Smekal, Sjaak Verdoold
   Copyright (C) 2005  Germain Carrera Corraleche
   Copyright (C) 1999  Frank Huebner

   This file is part of PLplot.

   PLplot is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   PLplot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with PLplot; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/


/* TODO: - implement the GraphicsIn stuff
 *       - clear screen should only clear current plot (e.g in a 2x2 plot) not the whole window
 */

#include "plDevs.h"

#ifdef PLD_wxwidgets

/* plplot headers */
#include "plplotP.h"
#include "drivers.h"

/* os specific headers */
#ifdef __WIN32__
  #include <windows.h>
#endif

#ifdef __WXMAC__
	#include <Carbon/Carbon.h>
  extern "C" { void CPSEnableForegroundOperation(ProcessSerialNumber* psn); }
#endif

/* C/C++ headers */
#include <cstdio>

/* wxwidgets headers */
#include "wx/wx.h"
#include "wx/except.h"
#include "wx/image.h"
#include "wx/filedlg.h"
#include "wx/display.h"
  
/* antigrain headers (for antialzing) */
#ifdef HAVE_AGG
  #include "agg2/agg_basics.h"
  #include "agg2/agg_rendering_buffer.h"
  #include "agg2/agg_rasterizer_scanline_aa.h"
  #include "agg2/agg_scanline_u.h"
  #include "agg2/agg_conv_stroke.h"
  #include "agg2/agg_pixfmt_rgb.h"
  #include "agg2/agg_renderer_base.h"
  #include "agg2/agg_renderer_scanline.h"
  #include "agg2/agg_path_storage.h"
  
  typedef agg::pixfmt_rgb24 pixfmt;
  typedef agg::renderer_base<pixfmt> ren_base;
  typedef agg::renderer_scanline_aa_solid<ren_base> renderer;
#endif  
  
/* freetype headers and macros */
#ifdef HAVE_FREETYPE
  #include "plfreetype.h"

  static void plD_pixel_wxwidgets( PLStream *pls, short x, short y );
  static PLINT plD_read_pixel_wxwidgets( PLStream *pls, short x, short y );
  static void plD_set_pixel_wxwidgets( PLStream *pls, short x, short y, PLINT colour );
  static void init_freetype_lv1( PLStream *pls );
  static void init_freetype_lv2( PLStream *pls );
extern "C"
{
  extern void plD_FreeType_init( PLStream *pls );
  extern void plD_render_freetype_text( PLStream *pls, EscText *args );
  extern void plD_FreeType_Destroy( PLStream *pls );
  extern void pl_set_extended_cmap0( PLStream *pls, int ncol0_width, int ncol0_org );
}
#endif

#ifndef max_number_of_grey_levels_used_in_text_smoothing
  #define max_number_of_grey_levels_used_in_text_smoothing 64
#endif

#define NDEV	100		/* Max number of output device types in menu */
// WX_DECLARE_STRING_HASH_MAP( NDEV, mDevFileEnding );
// mDevFileEnding

/*=========================================================================*/
/* Physical dimension constants used by the driver */

/* Virtual coordinate scaling parameter, used to do calculations at
 * higher resolution.  Chosen to be 32 for consistency with the PLplot
 * metafile (see plplotP.h).
 *
 * The trick here is to do everything in device coordinates on the driver
 * side, but report/receive everything in virtual coordinates to/from the
 * PLplot core.
 */
#define VSCALE (32.)

/* pixels per mm */
#define DEVICE_PIXELS_PER_MM (3.4)
#define VIRTUAL_PIXELS_PER_MM (DEVICE_PIXELS_PER_MM*VSCALE)

/* mm per inch */
#define MM_PER_IN (25.4)

/* pixels per inch */
#define DEVICE_PIXELS_PER_IN (DEVICE_PIXELS_PER_MM*MM_PER_IN)
#define VIRTUAL_PIXELS_PER_IN (VIRTUAL_PIXELS_PER_MM*MM_PER_IN)

/* Default dimensions of the canvas (in inches) */
#define CANVAS_WIDTH (10.)
#define CANVAS_HEIGHT (7.5)

/* size of plot in pixels on screen if not given */
#define PLOT_WIDTH 800
#define PLOT_HEIGHT 600
/*=========================================================================*/

/* Application icon as XPM */
/* This free icon was taken from http://2pt3.com/news/twotone-icons-for-free/ */
static const char *graph[] = {
/* columns rows colors chars-per-pixel */
"16 16 4 2",
"   c black",
".  c #BA1825",
"X  c gray100",
"UX c None",
/* pixels */
"UX. . . . . . . . . . . . . . UX",
". . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . ",
". . . . . . . . . . . X X . . . ",
". . . . . . . . . . . X X . . . ",
". . . . . . . . . . . X X . . . ",
". . . . . X X . . . . X X . . . ",
". . . . . X X . . . . X X . . . ",
". . . . . X X . X X . X X . . . ",
". . . . . X X . X X . X X . . . ",
". . . . . X X . X X . X X . . . ",
". . . . . X X . X X . X X . . . ",
". . . X X X X X X X X X X . . . ",
". . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . ",
"UX. . . . . . . . . . . . . . UX"
};

/* These need to be distinguished since the handling is slightly different. */
#define LOCATE_INVOKED_VIA_API		1
#define LOCATE_INVOKED_VIA_DRIVER	2

/* struct which contains information about device */
class wxPLplotFrame;
class wxPLdev {
public: /* methods */
	wxPLdev( void );
	~wxPLdev( void );

public: /* variables */
  bool ready;
  bool ownGUI;
  bool waiting;
  bool resizing;
  bool exit;

  int comcount;
  
  wxDC* dc;
  wxBitmap* m_bitmap;
  wxImage* m_buffer;
  wxPLplotFrame* m_frame;
  PLINT width;
  PLINT height;
  PLINT bm_width;
  PLINT bm_height;

  PLINT xmin;
  PLINT xmax;
  PLINT ymin;
  PLINT ymax;

	PLFLT scalex;
	PLFLT scaley;

  bool plstate_width;   /* Flags indicating change of state before */
  bool plstate_color0;  /* device is fully initialized */
  bool plstate_color1;  /* taken from gcw driver */
  
  PLGraphicsIn gin;			/* Graphics input structure */
  int locate_mode;		  /* Set while in locate mode */
  bool	draw_xhair;		/* Set during xhair draws */

	/* clipping region */
	int clipminx, clipmaxx;
	int clipminy, clipmaxy;
	bool newclipregion;

  /* variables for antializing */
	int antialized;
	int freetype;
	int smooth_text;
#ifdef HAVE_AGG
  agg::rendering_buffer *m_rendering_buffer;

  double m_strokewidth;
  wxUint8 m_StrokeOpacity;
  unsigned char m_colredstroke;
  unsigned char m_colgreenstroke;
  unsigned char m_colbluestroke;
  unsigned char m_colredfill;
  unsigned char m_colgreenfill;
  unsigned char m_colbluefill;
#endif

  const char** devDesc;		/* Descriptive names for file-oriented devices.  Malloc'ed. */
  const char** devName;		/* Keyword names of file-oriented devices. Malloc'ed. */
	int ndev;
};

struct dev_entry {    
  wxString dev_name;
  wxString dev_menu_short;
  wxString dev_menu_long;
  wxString dev_file_app;
};

struct dev_entry dev_entries[] = {
  { wxT("gif"), wxT("gif..."), wxT("Save this plot as gif!"), wxT("gif files (*.gif)|*.gif") },
  { wxT("jpeg"), wxT("jpeg..."), wxT("Save this plot as jpeg!"), wxT("jpg files (*.jpg;*.jpeg)|*.jpg;*.jpeg") },
  { wxT("png"), wxT("png..."), wxT("Save this plot as png"), wxT("png files (*.png)|*.png") },
  { wxT("pngcairo"), wxT("png (cairo)..."), wxT("Save this plot as png using cairo!"), wxT("png files (*.png)|*.png") },
  { wxT("pdfcairo"), wxT("pdf..."), wxT("Save this plot as pdf using cairo!"), wxT("pdf files (*.pdf)|*.pdf") },
  { wxT("ps"), wxT("postscript..."), wxT("Save this plot as postscript!"), wxT("ps files (*.ps)|*.ps") },
  { wxT("psc"), wxT("color postscript..."), wxT("Save this plot as color postscript!"), wxT("ps files (*.ps;*.psc)|*.ps;*.psc") },
  { wxT("pscairo"), wxT("color postscript (cairo)..."), wxT("Save this plot as color postscript using cairo!"), wxT("ps files (*.ps;*.psc)|*.ps;*.psc") },
  { wxT("svg"), wxT("svg..."), wxT("Save this plot as svg!"), wxT("svg files (*.svg)|*.svg") },
  { wxT("svgcairo"), wxT("svg (cairo)..."), wxT("Save this plot as svg using cairo!"), wxT("svg files (*.svg)|*.svg") },
  { wxT("xfig"), wxT("xfig..."), wxT("Save this plot as xfig!"), wxT("fig files (*.fig)|*.fig") }
};

inline void AddtoClipRegion( wxPLdev* dev, int x1, int y1, int x2, int y2 )
{
	dev->newclipregion=false;
	if( x1<x2 ) {
		if( x1<dev->clipminx ) dev->clipminx=x1;
		if( x2>dev->clipmaxx ) dev->clipmaxx=x2;
	} else {
		if( x2<dev->clipminx ) dev->clipminx=x2;
		if( x1>dev->clipmaxx ) dev->clipmaxx=x1;
	}
	if( y1<y2 ) {
		if( y1<dev->clipminy ) dev->clipminy=y1;
		if( y2>dev->clipmaxy ) dev->clipmaxy=y2;
	} else {
		if( y2<dev->clipminy ) dev->clipminy=y2;
		if( y1>dev->clipmaxy ) dev->clipmaxy=y1;
	}
}

/* after how many commands the window should be refreshed */
#define MAX_COMCOUNT 5000

/* wxwidgets application definition (if needed) */
WX_DEFINE_ARRAY( wxPLplotFrame*, wxArrayOfwxPLplotFrame );
class wxPLplotApp : public wxApp
{
public:
  virtual bool OnInit();
  /* virtual int OnExit(); */
  void SetExitFlag( bool flag=true ) { exit=flag; };
  bool GetExitFlag( void ) { return exit; };
  void SetAdvanceFlag( bool flag=true ) { advance=flag; };
  bool GetAdvanceFlag( void ) { return advance; };
  void SetRefreshFlag( bool flag=true );
	void AddFrame( wxPLplotFrame* frame ) { FrameArray.Add( frame ); };
	void RemoveFrame( wxPLplotFrame* frame ) { FrameArray.Remove( frame ); };
  size_t FrameCount( void ) { return FrameArray.GetCount(); }
  void OnIdle( wxIdleEvent& event );

private:
  bool exit;
  bool advance;
	wxArrayOfwxPLplotFrame FrameArray;

  DECLARE_EVENT_TABLE()
};

/* event table */
BEGIN_EVENT_TABLE( wxPLplotApp, wxApp )
  EVT_IDLE( wxPLplotApp::OnIdle )
END_EVENT_TABLE()

/* definition of the actual window/frame shown */
class wxPLplotWindow : public wxWindow
{
public:
	wxPLplotWindow( wxWindow* parent, PLStream *pls );

  void SetRefreshFlag( bool flag=true ) { refresh=flag; };
  bool GetRefreshFlag( void ) { return refresh; };
  
private:
  void OnPaint( wxPaintEvent& event );
  void OnChar( wxKeyEvent& event );
  void OnIdle( wxIdleEvent& event );
	void OnErase( wxEraseEvent& WXUNUSED(event) );
  void OnSize( wxSizeEvent & WXUNUSED(event) );
  void OnMaximize( wxMaximizeEvent & WXUNUSED(event) );
  void OnMouse( wxMouseEvent& event );
  void DrawCrosshair();
  
  PLStream *m_pls;
  wxPLdev* m_dev;  /* windows needs to know this structure */
  bool refresh;
  bool xhair_drawn;
  int mouse_x, mouse_y, old_mouse_x, old_mouse_y;

  DECLARE_EVENT_TABLE()
};

/* event table */
BEGIN_EVENT_TABLE( wxPLplotWindow, wxWindow )
  EVT_PAINT( wxPLplotWindow::OnPaint )               /* (re)draw the plot in window */
  EVT_CHAR( wxPLplotWindow::OnChar )
  EVT_IDLE( wxPLplotWindow::OnIdle )
  EVT_MOUSE_EVENTS( wxPLplotWindow::OnMouse )
	EVT_ERASE_BACKGROUND( wxPLplotWindow::OnErase )
  EVT_SIZE( wxPLplotWindow::OnSize )
  EVT_MAXIMIZE( wxPLplotWindow::OnMaximize )
END_EVENT_TABLE()

/* definition of the actual window/frame shown */

class wxPLplotFrame : public wxFrame
{
public:
  wxPLplotFrame( const wxString& title, PLStream* pls );
  void OnMenu( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );
  bool SavePlot( const char* filename, const char* devname, int width,  int height );
  void SetRefreshFlag( bool flag=true ) { m_window->SetRefreshFlag( flag ); };
  bool GetRefreshFlag( void ) { return m_window->GetRefreshFlag(); };

private:
  wxPanel* m_panel;
  wxPLplotWindow* m_window;
  wxPLdev* m_dev;  /* frame needs to know this structure */

  DECLARE_EVENT_TABLE()
};

enum { wxPL_Save=10000, wxPL_Next=10100 };

/* event table */
BEGIN_EVENT_TABLE( wxPLplotFrame, wxFrame )
  EVT_MENU( -1, wxPLplotFrame::OnMenu )      /* handle all menu events */
  EVT_CLOSE( wxPLplotFrame::OnClose )
END_EVENT_TABLE()

/* Use this macro if you want to define your own main() or WinMain() function
   and call wxEntry() from there. */
#define IMPLEMENT_PLAPP_NO_MAIN(appname)                                      \
    wxAppConsole *wxPLCreateApp()                                             \
    {                                                                       \
        wxAppConsole::CheckBuildOptions( WX_BUILD_OPTIONS_SIGNATURE,         \
                                         "your program" );                    \
        return new appname;                                                 \
    }                                                                       \
    wxAppInitializer                                                        \
        wxAppInitializer((wxAppInitializerFunction) wxPLCreateApp);        \
    static appname& wxGetApp() { return *(appname *)wxTheApp; } 

IMPLEMENT_PLAPP_NO_MAIN( wxPLplotApp )

/* workaround against warnings for unused variables */
static inline void Use(void *) { }
#define WX_SUPPRESS_UNUSED_WARN( x ) Use( &x )

/* private functions needed by the wxwidgets Driver */
static void install_buffer( PLStream *pls );
static void wxRunApp( PLStream *pls, bool runonce=false );
static void GetCursorCmd( PLStream *pls, PLGraphicsIn *ptr );

/*----------------------------------------------------------------------*\
 *  Declarations for the device.
\*----------------------------------------------------------------------*/

/* Device info */
const char* plD_DEVICE_INFO_wxwidgets = "wxwidgets:wxWidgets DC:1:wxwidgets:51:wxwidgets";

void plD_init_wxwidgets		(PLStream *);
void plD_line_wxwidgets		(PLStream *, short, short, short, short);
void plD_polyline_wxwidgets	(PLStream *, short *, short *, PLINT);
void plD_eop_wxwidgets		(PLStream *);
void plD_bop_wxwidgets		(PLStream *);
void plD_tidy_wxwidgets		(PLStream *);
void plD_state_wxwidgets	(PLStream *, PLINT);
void plD_esc_wxwidgets		(PLStream *, PLINT, void *);

static void fill_polygon( PLStream *pls );
void wx_set_dc( PLStream* pls, wxDC* dc );
void wx_set_buffer( PLStream* pls, wxImage* buffer );
void wx_set_size( PLStream* pls, int width, int height );
int plD_errorexithandler_wxwidgets( char *errormessage );
void plD_erroraborthandler_wxwidgets( char *errormessage );


/*----------------------------------------------------------------------*\
 *  Debug functions
\*----------------------------------------------------------------------*/

/* define if you want debug output */
/* #define _DEBUG //*/
/* #define _DEBUG_VERBOSE //*/

/*--------------------------------------------------------------------------*\
 *  void Log_Verbose( const char *fmt, ... )
 *
 *  Print verbose debug message to stderr (printf style).
\*--------------------------------------------------------------------------*/
void Log_Verbose( const char *fmt, ... )
{
#ifdef _DEBUG_VERBOSE
  va_list args;
  va_start( args, fmt );
  fprintf( stderr, "Verbose: " );
  vfprintf( stderr, fmt, args );
  fprintf( stderr, "\n" );
  va_end( args );
  fflush( stderr );
#endif
}


/*--------------------------------------------------------------------------*\
 *  void Log_Debug( const char *fmt, ... )
 *
 *  Print debug message to stderr (printf style).
\*--------------------------------------------------------------------------*/
void Log_Debug( const char *fmt, ... )
{
#ifdef _DEBUG
  va_list args;
  va_start( args, fmt );
  fprintf( stderr, "Debug: " );
  vfprintf( stderr, fmt, args );
  fprintf( stderr, "\n" );
  va_end( args );
  fflush( stderr );
#endif
}





/*----------------------------------------------------------------------*\
 *  In the following you'll find the driver functions which are
 *  are needed by the plplot core.
\*----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
 *  void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
 *
 *  Make driver functions known to plplot.
\*--------------------------------------------------------------------------*/
void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr  = "wxWidgets DC";
    pdt->pl_DevName  = "wxwidgets";
#endif
    pdt->pl_type     = plDevType_Interactive;
    pdt->pl_seq      = 51;
    pdt->pl_init     = (plD_init_fp)     plD_init_wxwidgets;
    pdt->pl_line     = (plD_line_fp)     plD_line_wxwidgets;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wxwidgets;
    pdt->pl_eop      = (plD_eop_fp)      plD_eop_wxwidgets;
    pdt->pl_bop      = (plD_bop_fp)      plD_bop_wxwidgets;
    pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_wxwidgets;
    pdt->pl_state    = (plD_state_fp)    plD_state_wxwidgets;
    pdt->pl_esc      = (plD_esc_fp)      plD_esc_wxwidgets;
}


wxPLdev::wxPLdev( void )
{
  Log_Verbose( "wxPLdev::wxPLdev()" );

  ready = false;
  ownGUI = false;
  waiting = false;
  resizing = false;
  exit=false;

  comcount = 0;

	dc=NULL;
  m_bitmap=NULL;
  m_buffer=NULL;
  m_frame=NULL;
  // width, height are set in plD_init_wxwidgets
  // bm_width, bm_height are set in install_buffer
  
  // xmin, xmax, ymin, ymax are set in plD_init_wxwidgets
  // scalex, scaley are set in plD_init_wxwidgets

  plstate_width = false;
  plstate_color0 = false;
  plstate_color1 = false;

  locate_mode=0;
  draw_xhair=false;
  
	newclipregion=true;
	clipminx=1024;
	clipmaxx=0;
	clipminy=800;
	clipmaxy=0;

	antialized=0;
	freetype=0;
	smooth_text=0;
#ifdef HAVE_AGG
  m_rendering_buffer=NULL;
  m_strokewidth=1.0;
  m_StrokeOpacity=255;
  m_colredstroke=255;
  m_colgreenstroke=255;
  m_colbluestroke=255;
  m_colredfill=0;
  m_colgreenfill=0;
  m_colbluefill=0;
#endif

  devName = (const char **)malloc( NDEV * sizeof(char**) );
  memset( devName, '\0', NDEV * sizeof(char**) );
  devDesc = (const char **)malloc( NDEV * sizeof(char**) );
  memset( devDesc, '\0', NDEV * sizeof(char**) );
	ndev=NDEV;
}


wxPLdev::~wxPLdev( void )
{
  if( devDesc )
    free( devDesc );
  if( devName )
    free( devName );
}


/*--------------------------------------------------------------------------*\
 *  plD_init_wxwidgets()
 *
 *  Initialize device.
\*--------------------------------------------------------------------------*/
void
plD_init_wxwidgets( PLStream *pls )
{
  Log_Verbose( "plD_init_wxwidgets()" );

  wxPLdev* dev;

  /* default options */
#ifdef HAVE_FREETYPE
  static int freetype=1;
  static int smooth_text=1;
#endif
#ifdef HAVE_AGG
	static int antialized=0;
#endif

DrvOpt wx_options[] = {
#ifdef HAVE_FREETYPE
        {"text", DRV_INT, &freetype, "Use driver text (FreeType)"},
        {"smooth", DRV_INT, &smooth_text, "Turn text smoothing on (1) or off (0)"},
#endif
#ifdef HAVE_AGG
        {"antialized", DRV_INT, &antialized, "Turn antializing on (1) or off (0)"},
#endif
        {NULL, DRV_INT, NULL, NULL}};

/* be verbose and write out debug messages */
#ifdef _DEBUG
  pls->verbose = 1;
  pls->debug = 1;
#else
  pls->verbose = 0;
  pls->debug = 0;
#endif

  /* allocate memory for the device storage */
  dev = (wxPLdev*)new wxPLdev;
	if( dev == NULL) {
    fprintf( stderr, "Insufficient memory\n" );
    exit( 0 );
  }
  pls->dev = (void*)dev;

  /* Check for and set up driver options */
  plParseDrvOpts( wx_options );
#ifdef HAVE_FREETYPE
	dev->smooth_text=smooth_text;
	dev->freetype=freetype;
#endif
#ifdef HAVE_AGG
  dev->antialized=antialized;
#endif

  pls->color = 1;		/* Is a color device */
  pls->dev_fill0 = 1;		/* Can handle solid fills */
  pls->dev_fill1 = 0;		/* Can't handle pattern fills */
  pls->dev_dash = 0;
  pls->plbuf_write = 1;    /* use the plot buffer! */
  pls->termin = 1;             /* interactive device */
  pls->graphx = GRAPHICS_MODE; /*  No text mode for this driver (at least for now, might add a console window if I ever figure it out and have the inclination) */
  pls->dev_clear = 1;          /* driver supports clear */

#ifdef HAVE_FREETYPE
  if( freetype ) {
    pls->dev_text = 1; /* want to draw text */
    pls->dev_unicode = 1; /* want unicode */

    init_freetype_lv1( pls );
    FT_Data* FT=(FT_Data *)pls->FT;
    FT->want_smooth_text=smooth_text;
  }
#endif

  /* initialize device storage */
  if( pls->xlength <= 0 || pls->ylength <=0 )
    plspage( 0.0, 0.0, (PLINT)(CANVAS_WIDTH*DEVICE_PIXELS_PER_IN), (PLINT)(CANVAS_HEIGHT*DEVICE_PIXELS_PER_IN), 0, 0 );

  dev->width=pls->xlength;
  dev->height=pls->ylength;
  dev->clipminx=pls->xlength;
  dev->clipminy=pls->ylength;

  /* If portrait mode, apply a rotation and set freeaspect */
  if( pls->portrait ) {
    plsdiori( (PLFLT)(4 - ORIENTATION) );
    pls->freeaspect = 1;
  }

  Log_Verbose( "settings" );

  /* Set the number of pixels per mm */
  plP_setpxl( (PLFLT)VIRTUAL_PIXELS_PER_MM, (PLFLT)VIRTUAL_PIXELS_PER_MM );

  /* Set up physical limits of plotting device (in drawing units) */
  plP_setphy( (PLINT)0, (PLINT)(CANVAS_WIDTH*VIRTUAL_PIXELS_PER_IN),
	            (PLINT)0, (PLINT)(CANVAS_HEIGHT*VIRTUAL_PIXELS_PER_IN) );

  /* get physical device limits coordinates */
  plP_gphy( &dev->xmin, &dev->xmax, &dev->ymin, &dev->ymax );

  /* setting scale factors */
  dev->scalex=(PLFLT)(dev->xmax-dev->xmin)/(dev->width);
  dev->scaley=(PLFLT)(dev->ymax-dev->ymin)/(dev->height);  

  /* set dpi */    
  plspage(VIRTUAL_PIXELS_PER_IN/dev->scalex, VIRTUAL_PIXELS_PER_IN/dev->scaley, 0, 0, 0, 0);

	/* Set wx error handler for various errors in plplot*/
  /* plsexit(plD_errorexithandler_wxwidgets); */
  /* plsabort(plD_erroraborthandler_wxwidgets); */
  
#ifdef HAVE_FREETYPE
  if( pls->dev_text )
    init_freetype_lv2( pls );
#endif
  
  /* find out what file drivers are available */
  plgFileDevs( &dev->devDesc, &dev->devName, &dev->ndev );
}


/*--------------------------------------------------------------------------*\
 *  void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a,
 *													 short x2a, short y2a )
 *
 *  Draws a line from (x1a, y1a) to (x2a, y2a).
\*--------------------------------------------------------------------------*/
void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
  Log_Verbose( "plD_line_wxwidgets(x1a=%d, y1a=%d, x2a=%d, y2a=%d)", x1a, y1a, x2a, y2a );

  wxPLdev* dev = (wxPLdev*)pls->dev;

	x1a=(short)(x1a/dev->scalex); y1a=(short)(dev->height-y1a/dev->scaley);
	x2a=(short)(x2a/dev->scalex);	y2a=(short)(dev->height-y2a/dev->scaley);

  if( !(dev->ready) )
    install_buffer( pls );
  
  Log_Verbose( "plD_line_wxwidgets(x1a=%d, y1a=%d, x2a=%d, y2a=%d)", (wxCoord)(x1a/dev->scalex),
							 (wxCoord)(dev->height-y1a/dev->scaley), (wxCoord)(x2a/dev->scalex), (wxCoord)(dev->height-y2a/dev->scaley) );

  if(dev->antialized) {
#ifdef HAVE_AGG
    agg::rasterizer_scanline_aa<> ras;
    agg::scanline_u8 sl;
    pixfmt pixf( *dev->m_rendering_buffer );
    ren_base renb( pixf );
    renderer ren( renb );

    agg::path_storage path;
    path.move_to( x1a, y1a );
    path.line_to( x2a, y2a );

    agg::conv_stroke<agg::path_storage> stroke(path);
    stroke.line_join( agg::round_join );
    stroke.line_cap( agg::round_cap );
    stroke.width( dev->m_strokewidth );
    ras.add_path( stroke );
      
    ren.color( agg::rgba8(dev->m_colredstroke, dev->m_colgreenstroke, dev->m_colbluestroke, dev->m_StrokeOpacity) );
      
    agg::render_scanlines( ras, sl, ren );
#endif
  } else 
    dev->dc->DrawLine( (wxCoord)x1a, (wxCoord)y1a, (wxCoord)x2a, (wxCoord)y2a );
  
  if( !(dev->resizing) && dev->ownGUI ) {
		AddtoClipRegion( dev, (int)x1a, (int)y1a, (int)x2a, (int)y2a );
		dev->comcount+=10;
		if( dev->comcount>MAX_COMCOUNT ) {
			wxRunApp( pls, true );
			dev->comcount=0;
		}
	}
}


/*--------------------------------------------------------------------------*\
 *  void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya,
 *															 PLINT npts )
 *
 *  Draw a poly line - points are in xa and ya arrays.
\*--------------------------------------------------------------------------*/
void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya, PLINT npts )
{
	short x1a, y1a, x2a, y2a;
  Log_Verbose( "plD_polyline_wxwidgets()" );

  /* should be changed to use the wxDC::DrawLines function? */
  wxPLdev* dev = (wxPLdev*)pls->dev;

  if( !(dev->ready) )
    install_buffer( pls );

  if( dev->antialized ) {
#ifdef HAVE_AGG
    agg::rasterizer_scanline_aa<> ras;
    agg::scanline_u8 sl;
    pixfmt pixf( *dev->m_rendering_buffer );
    ren_base renb( pixf );
    renderer ren( renb );

    agg::path_storage path;
		x2a=(short)(xa[0]/dev->scalex); y2a=(short)(dev->height-ya[0]/dev->scaley);
    path.move_to( x2a, y2a );
    for ( PLINT i=1; i<npts; i++ ) {
			x1a=x2a; y1a=y2a;
			x2a=(short)(xa[i]/dev->scalex); y2a=(short)(dev->height-ya[i]/dev->scaley);
      path.line_to( x2a, y2a );
			if( !(dev->resizing) && dev->ownGUI ) 
				AddtoClipRegion( dev, (int)x1a, (int)y1a, (int)x2a, (int)y2a );
		}

    agg::conv_stroke<agg::path_storage> stroke( path );
    stroke.line_join( agg::round_join );
    stroke.line_cap( agg::round_cap );
    stroke.width( dev->m_strokewidth );
    ras.add_path( stroke );

    ren.color( agg::rgba8(dev->m_colredstroke, dev->m_colgreenstroke, dev->m_colbluestroke, dev->m_StrokeOpacity) );
    
    agg::render_scanlines( ras, sl, ren );
#endif
 } else {
		x2a=(short)(xa[0]/dev->scalex); y2a=(short)(dev->height-ya[0]/dev->scaley);
    for( PLINT i=1; i<npts; i++ ) {
			x1a=x2a; y1a=y2a;
			x2a=(short)(xa[i]/dev->scalex); y2a=(short)(dev->height-ya[i]/dev->scaley);

      dev->dc->DrawLine( (wxCoord)x1a, (wxCoord)y1a, (wxCoord)x2a, (wxCoord)y2a );
			if( !(dev->resizing) && dev->ownGUI ) 
				AddtoClipRegion( dev, (int)x1a, (int)y1a, (int)x2a, (int)y2a );
		}
  }

  if( !(dev->resizing) && dev->ownGUI ) {
    dev->comcount+=10;
		if( dev->comcount>MAX_COMCOUNT ) {
			wxRunApp( pls, true );
			dev->comcount=0;
		}
	}
}


/*--------------------------------------------------------------------------*\
 *  void plD_eop_wxwidgets( PLStream *pls )
 *
 *  End of Page. This function is called if a "end of page" is send by the
 *  user. This command is ignored if we have the plot embedded in a 
 *  wxWidgets application, but a new bitmap is created if we use a
 *  command line executable.
\*--------------------------------------------------------------------------*/
void plD_eop_wxwidgets( PLStream *pls )
{
  Log_Verbose( "plD_eop_wxwidgets()" );

  wxPLdev* dev = (wxPLdev*)pls->dev;

  if( dev->ownGUI )
    if ( pls->nopause )
      wxRunApp( pls, true );
    else
      wxRunApp( pls );
}


/*--------------------------------------------------------------------------*\
 *  void plD_bop_wxwidgets( PLStream *pls )
 *
 *  Begin of page. Before any plot command, this function is called, If we
 *  have already a dc the background is cleared in background color and some
 *  state calls are resent - this is because at the first call of this
 *  function, a dc does most likely not exist, but state calls are recorded
 *  and when a new dc is created this function is called again.
\*--------------------------------------------------------------------------*/
void plD_bop_wxwidgets( PLStream *pls )
{
  Log_Verbose( "plD_bop_wxwidgets()" );

  wxPLdev* dev = (wxPLdev*)pls->dev;

  if( dev->ready ) {
    /* clear background */
		PLINT bgr, bgg, bgb;  /* red, green, blue */
		plgcolbg( &bgr, &bgg, &bgb);  /* get background color information */

    if( dev->antialized ) {
#ifdef HAVE_AGG
      pixfmt pixf( *dev->m_rendering_buffer );
      ren_base renb( pixf );
      renb.clear( agg::rgba8(bgr, bgg, bgb) );
#endif
    } else {
      dev->dc->SetBackground( wxBrush(wxColour(bgr, bgg, bgb)) );
      dev->dc->Clear();
    }
		AddtoClipRegion( dev, 0, 0, (int)(dev->width), (int)(dev->height) );
    
    /* Replay escape calls that come in before PLESC_DEVINIT.  All of them
     * required a DC that didn't exist yet.
     */
    if( dev->plstate_width )
      plD_state_wxwidgets( pls, PLSTATE_WIDTH );
    dev->plstate_width = false;

    if( dev->plstate_color0 )
      plD_state_wxwidgets( pls, PLSTATE_COLOR0 );
    dev->plstate_color0 = false;

    if( dev->plstate_color1 )
      plD_state_wxwidgets( pls, PLSTATE_COLOR1 );
    dev->plstate_color1 = false;

		/* why this? xwin driver has this */
	  /* pls->page++; */
	}
}


/*--------------------------------------------------------------------------*\
 *  void plD_tidy_wxwidgets( PLStream *pls )
 *
 *  This function is called, if all plots are done. 
\*--------------------------------------------------------------------------*/
void plD_tidy_wxwidgets( PLStream *pls )
{
  Log_Verbose( "plD_tidy_wxwidgets()" );

  wxPLdev* dev = (wxPLdev*)pls->dev;

#ifdef HAVE_FREETYPE
  if( pls->dev_text ) {
    FT_Data *FT=(FT_Data *)pls->FT;
    plscmap0n( FT->ncol0_org );
    plD_FreeType_Destroy( pls );
  }
#endif

  if( dev->ownGUI ) {
    if( dev->antialized ) {
      delete dev->m_buffer;
#ifdef HAVE_AGG
      delete dev->m_rendering_buffer;
#endif
    } else {
      ((wxMemoryDC*)dev->dc)->SelectObject( wxNullBitmap );
      delete dev->dc;
      delete dev->m_bitmap;
    }
    
    wxGetApp().RemoveFrame( dev->m_frame );
		if( !wxGetApp().FrameCount() )
    	wxUninitialize();
  }
  
  delete dev;
  pls->dev=NULL;  /* since in plcore.c pls->dev is free_mem'd */
}


/*--------------------------------------------------------------------------*\
 *  void plD_state_wxwidgets( PLStream *pls, PLINT op )
 *
 *  Handler for several state codes. Here we take care of setting the width
 *  and color of the pen.
\*--------------------------------------------------------------------------*/
void plD_state_wxwidgets( PLStream *pls, PLINT op )
{
  Log_Verbose( "plD_state_wxwidgets(op=%d)", op );

  wxPLdev* dev = (wxPLdev*)pls->dev;

  switch( op ) {
		
  case PLSTATE_WIDTH:  /* 1 */
    if( dev->ready ) {
      if( dev->antialized ) {
#ifdef HAVE_AGG
        dev->m_strokewidth = pls->width>0 ? pls->width : 1;  // TODO: why and when ist width 0???
#endif
      } else {
        dev->dc->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->curcolor.r, pls->curcolor.g,
                                                                   pls->curcolor.b),
                                                          pls->width>0 ? pls->width : 1, wxSOLID)) );
      }
    } else
      dev->plstate_width = true;
    break;
		
  case PLSTATE_COLOR0:  /* 2 */
    if( dev->ready ) {
      if( dev->antialized ) {
#ifdef HAVE_AGG
        dev->m_colredstroke = pls->cmap0[pls->icol0].r;
        dev->m_colgreenstroke = pls->cmap0[pls->icol0].g;
        dev->m_colbluestroke = pls->cmap0[pls->icol0].b;
        dev->m_StrokeOpacity = (wxUint8)(pls->cmap0[pls->icol0].a*255);
#endif
      } else {
        dev->dc->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->cmap0[pls->icol0].r, pls->cmap0[pls->icol0].g,
                                                                    pls->cmap0[pls->icol0].b),
                                                          pls->width>0 ? pls->width : 1, wxSOLID)) );
        dev->dc->SetBrush( wxBrush(wxColour(pls->cmap0[pls->icol0].r, pls->cmap0[pls->icol0].g,
                                            pls->cmap0[pls->icol0].b)) );
      }
    } else
      dev->plstate_color0 = true;
    break;
		
  case PLSTATE_COLOR1:  /* 3 */
    if( dev->ready ) {
      if( dev->antialized ) {
#ifdef HAVE_AGG
        dev->m_colredstroke = pls->curcolor.r;
        dev->m_colgreenstroke = pls->curcolor.g;
        dev->m_colbluestroke = pls->curcolor.b;      
        dev->m_StrokeOpacity = (wxUint8)(pls->curcolor.a*255);
#endif
      } else {
        dev->dc->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(pls->curcolor.r, pls->curcolor.g,
                                                                    pls->curcolor.b),
                                                          pls->width>0 ? pls->width : 1, wxSOLID)) );
        dev->dc->SetBrush( wxBrush(wxColour(pls->curcolor.r, pls->curcolor.g, pls->curcolor.b)) );
      }
    } else 
      dev->plstate_color1 = true;
    break;
		
  default:
    if( !(dev->ready) )
      install_buffer( pls );
  }
}


/*--------------------------------------------------------------------------*\
 *  void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
 *
 *  Handler for several escape codes. Here we take care of filled polygons,
 *  XOR or copy mode, initialize device (install dc from outside), and if 
 *  there is freetype support, rerendering of text.
\*--------------------------------------------------------------------------*/
void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
{
  Log_Verbose( "plD_esc_wxwidgets(op=%d, ptr=%x)", op, ptr );

  wxPLdev* dev = (wxPLdev*)pls->dev;

  switch (op) {

  case PLESC_FILL:
    fill_polygon( pls );
    break;

	case PLESC_XORMOD:
		/* switch between wxXOR and wxCOPY */
    if( dev->ready ) {
			if( dev->dc->GetLogicalFunction() == wxCOPY )
				dev->dc->SetLogicalFunction( wxXOR );    
			else if( dev->dc->GetLogicalFunction() == wxXOR )
				dev->dc->SetLogicalFunction( wxCOPY );
		}
		break;

	case PLESC_DEVINIT:
    if( dev->antialized )
      wx_set_buffer( pls, (wxImage*)ptr );
    else
      wx_set_dc( pls, (wxDC*)ptr );      
    break;

#ifdef HAVE_FREETYPE
  case PLESC_HAS_TEXT:
    plD_render_freetype_text( pls, (EscText *)ptr );
    break;
#endif

  case PLESC_RESIZE: {
    	wxSize* size=(wxSize*)ptr;
    	wx_set_size( pls, size->GetWidth(), size->GetHeight() );
		}
    break;

	case PLESC_CLEAR: {
    /* Since the plot is updated only every MAC_COMCOUNT commands (usually 5000)
       before we clear the screen we need to show the plot at least once :) */
    if( dev->ownGUI ) {
        wxRunApp( pls, true );
  			dev->comcount=0;
    }
/*				"x1", (double) (pls->sppxmi) * PIXELS_PER_DU,
				"y1", (double) -(pls->sppyma) * PIXELS_PER_DU,
				"x2", (double) (pls->sppxma) * PIXELS_PER_DU,
				"y2", (double) -(pls->sppymi) * PIXELS_PER_DU,
				"fill_color", (pls->cmap0[0]).name,*/
			PLINT bgr, bgg, bgb;  /* red, green, blue */
			plgcolbg( &bgr, &bgg, &bgb );  /* get background color information */

      if( dev->antialized ) {
#ifdef HAVE_AGG
        pixfmt pixf( *dev->m_rendering_buffer );
        ren_base renb( pixf );
        renb.clear( agg::rgba8(bgr, bgg, bgb) );
#endif
      } else {
        dev->dc->SetBackground( wxBrush(wxColour(bgr, bgg, bgb)) );
        dev->dc->Clear();
      }
  		AddtoClipRegion( dev, 0, 0, (int)( dev->width), (int)(dev->height) );
		}
		break;
  
  case PLESC_GETC:
	  GetCursorCmd( pls, (PLGraphicsIn *) ptr );
	  break;

  default:
    break;
  }
}


/*--------------------------------------------------------------------------*\
 *  static void fill_polygon( PLStream *pls )
 *
 *  Fill polygon described in points pls->dev_x[] and pls->dev_y[].
\*--------------------------------------------------------------------------*/
static void fill_polygon( PLStream *pls )
{
	short x1a, y1a, x2a, y2a;
  Log_Verbose( "fill_polygon(), npts=%d, x[0]=%d, y[0]=%d", pls->dev_npts, pls->dev_y[0], pls->dev_y[0] );

  wxPLdev* dev = (wxPLdev*)pls->dev;

  if( !(dev->ready) )
    install_buffer( pls );

  if( dev->antialized ) {
#ifdef HAVE_AGG
    agg::rasterizer_scanline_aa<> ras;
    agg::scanline_u8 sl;
    pixfmt pixf( *dev->m_rendering_buffer );
    ren_base renb(pixf);
    renderer ren(renb);

    agg::path_storage path;
    x2a=(short)(pls->dev_x[0]/dev->scalex); y2a=(short)(dev->height-pls->dev_y[0]/dev->scaley);
    path.move_to( x2a, y2a );
    for ( PLINT i=1; i<pls->dev_npts; i++ ) {
      x1a=x2a; y1a=y2a;
      x2a=(short)(pls->dev_x[i]/dev->scalex); y2a=(short)(dev->height-pls->dev_y[i]/dev->scaley);
      path.line_to( x2a, y2a );
      if( !(dev->resizing) && dev->ownGUI ) 
        AddtoClipRegion( dev, (int)x1a, (int)y1a, (int)x2a, (int)y2a );
    }
    path.line_to( (pls->dev_x[0]/dev->scalex), (dev->height-pls->dev_y[0]/dev->scaley) );
    path.close_polygon();

    ren.color( agg::rgba8(dev->m_colredstroke, dev->m_colgreenstroke, dev->m_colbluestroke, dev->m_StrokeOpacity) );
    ras.add_path( path );
    agg::render_scanlines(ras, sl, ren);

    agg::conv_stroke<agg::path_storage> stroke(path);
    stroke.line_join( agg::round_join );
    stroke.line_cap( agg::round_cap );
    stroke.width( dev->m_strokewidth );
    ras.add_path( stroke );    
    ren.color( agg::rgba8(dev->m_colredstroke, dev->m_colgreenstroke, dev->m_colbluestroke, dev->m_StrokeOpacity) );
    agg::render_scanlines( ras, sl, ren );
#endif
  } else {
    wxPoint *points = new wxPoint[pls->dev_npts];
  
    for( int i=0; i < pls->dev_npts; i++ ) {
      points[i].x=(int) (pls->dev_x[i]/dev->scalex);
      points[i].y=(int) (dev->height-pls->dev_y[i]/dev->scaley);
    }
  
    dev->dc->DrawPolygon( pls->dev_npts, points );
    delete[] points;
  }

  if( !(dev->resizing) && dev->ownGUI ) {
    dev->comcount+=10;
		if( dev->comcount>MAX_COMCOUNT ) {
			wxRunApp( pls, true );
			dev->comcount=0;
		}
	}
}


/*--------------------------------------------------------------------------*\
 *  void wx_set_size( PLStream* pls, int width, int height )
 *
 *  Adds a dc to the stream. The associated device is attached to the canvas
 *  as the property "dev".  
\*--------------------------------------------------------------------------*/
void wx_set_size( PLStream* pls, int width, int height )
{
  /* TODO: buffer must be resized here or in wxplotstream */
  Log_Verbose( "wx_set_size()" );
  
  wxPLdev* dev = (wxPLdev*)pls->dev;

	/* set new size */
	dev->width = width;
	dev->height = height;

  /* clear background if we have a dc, since it's invalid (TODO: why, since in bop
     it must be cleared anyway?) */
  if( dev->ready ) {
		PLINT bgr, bgg, bgb;  /* red, green, blue */
		plgcolbg( &bgr, &bgg, &bgb);  /* get background color information */

    if( dev->antialized ) {
#ifdef HAVE_AGG
		  if( dev->m_rendering_buffer )
			  delete dev->m_rendering_buffer;
		  dev->m_rendering_buffer = new agg::rendering_buffer;
			if( dev->ownGUI )
				dev->m_rendering_buffer->attach( dev->m_buffer->GetData(), dev->bm_width, dev->bm_height, dev->bm_width*3 );
			else
				dev->m_rendering_buffer->attach( dev->m_buffer->GetData(), dev->width, dev->height, dev->width*3 );
      pixfmt pixf( *dev->m_rendering_buffer );
      ren_base renb( pixf );
      renb.clear( agg::rgba8(bgr, bgg, bgb) );
#endif
    } else {
      dev->dc->SetBackground( wxBrush(wxColour(bgr, bgg, bgb)) );
      dev->dc->Clear();
    }
		AddtoClipRegion( dev, 0, 0, (int)( dev->width), (int)(dev->height) );
	}

  dev->scalex=(PLFLT)(dev->xmax-dev->xmin)/dev->width;
  dev->scaley=(PLFLT)(dev->ymax-dev->ymin)/dev->height;  
    
    /* recalculate the dpi used in calculation of fontsize */
  pls->xdpi = VIRTUAL_PIXELS_PER_IN/dev->scalex;
  pls->ydpi = VIRTUAL_PIXELS_PER_IN/dev->scaley;
    
  /* freetype parameters must also be changed */
#ifdef HAVE_FREETYPE  
  if( pls->dev_text ) {
    FT_Data *FT=(FT_Data *)pls->FT;
    FT->scalex=dev->scalex;   
    FT->scaley=dev->scaley;   
    FT->ymax=dev->height;
  }
#endif
}


/*--------------------------------------------------------------------------*\
 *  void wx_set_dc( PLStream* pls, wxDC* dc )
 *
 *  Adds a dc to the stream. The associated device is attached to the canvas
 *  as the property "dev".
\*--------------------------------------------------------------------------*/
void wx_set_dc( PLStream* pls, wxDC* dc )
{
  Log_Verbose( "wx_set_dc()" );
  
  wxPLdev* dev = (wxPLdev*)pls->dev;
  dev->dc=dc;  /* Add the dc to the device */
  dev->ready = true;
  dev->ownGUI = false;

	/* replay begin of page call and state settings */
	plD_bop_wxwidgets( pls );
}


/*--------------------------------------------------------------------------*\
 *  void wx_set_buffer( PLStream* pls, wxImage* dc )
 *
 *  Adds a dc to the stream. The associated device is attached to the canvas
 *  as the property "dev".
\*--------------------------------------------------------------------------*/
void wx_set_buffer( PLStream* pls, wxImage* buffer )
{
  Log_Verbose( "wx_set_buffer()" );
  
  wxPLdev* dev = (wxPLdev*)pls->dev;
  dev->m_buffer = buffer;
#ifdef HAVE_AGG
  if( dev->m_rendering_buffer )
    delete dev->m_rendering_buffer;
  dev->m_rendering_buffer = new agg::rendering_buffer;
  dev->m_rendering_buffer->attach( dev->m_buffer->GetData(), dev->width, dev->height, dev->width*3 );
#endif
  dev->ready = true;
  dev->ownGUI = false;

	/* replay begin of page call and state settings */
	plD_bop_wxwidgets( pls );
}


/*----------------------------------------------------------------------*\
 *  int plD_errorexithandler_wxwidgets( char *errormessage )
 *
 *  If an PLplot error occurs, this function shows a dialog regarding
 *  this error and than exits.
\*----------------------------------------------------------------------*/
int plD_errorexithandler_wxwidgets( char *errormessage )
{  
  /* wxPLdev* dev = (wxPLdev*)pls->dev; */

  /* if( dev->ownGUI ) { */
    wxMessageDialog dialog( 0, wxString(errormessage, *wxConvCurrent),wxString("wxPlot error",*wxConvCurrent),wxOK );
    dialog.ShowModal();
    plend();
    return 0;
  /*} */
}

/*----------------------------------------------------------------------*\
 *  void plD_erroraborthandler_wxwidgets( char *errormessage )
 *
 *  If an PLplot error occurs, this function shows a dialog regarding
 *  this error.
\*----------------------------------------------------------------------*/
void plD_erroraborthandler_wxwidgets( char *errormessage )
{  
  /* wxPLdev* dev = (wxPLdev*)pls->dev; */

  /* if( dev->ownGUI ) { */
    wxMessageDialog dialog( 0,(wxString(errormessage, *wxConvCurrent)+ wxString(" aborting operation...", *wxConvCurrent)), wxString("wxPlot error",*wxConvCurrent), wxOK );
    dialog.ShowModal();
  /* } */
}




#ifdef HAVE_FREETYPE

/*----------------------------------------------------------------------*\
 *  static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
 *
 *  callback function, of type "plD_pixel_fp", which specifies how a single
 *  pixel is set in the current colour.
\*----------------------------------------------------------------------*/
static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
{
  Log_Verbose( "plD_pixel_wxwidgets" );

  wxPLdev *dev=(wxPLdev*)pls->dev;

  if( !(dev->ready) )
    install_buffer( pls );

  if( dev->antialized ) {
#ifdef HAVE_AGG
    dev->m_buffer->SetRGB( x, y, dev->m_colredstroke, dev->m_colgreenstroke, dev->m_colbluestroke );
#endif
  } else
    dev->dc->DrawPoint( x, y );

  if( !(dev->resizing) && dev->ownGUI ) {
		AddtoClipRegion( dev, x, y, x+1, y+1 );
    dev->comcount++;
		if( dev->comcount>MAX_COMCOUNT ) {
			wxRunApp( pls, true );
			dev->comcount=0;
		}
	}
}


/*----------------------------------------------------------------------*\
 *  static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
 *
 *  callback function, of type "plD_pixel_fp", which specifies how a single
 *  pixel is set in the current colour.
\*----------------------------------------------------------------------*/
static void plD_set_pixel_wxwidgets( PLStream *pls, short x, short y, PLINT colour)
{
  Log_Verbose( "plD_set_pixel_wxwidgets" );

  wxPLdev *dev=(wxPLdev*)pls->dev;

  if( !(dev->ready) )
    install_buffer( pls );

  if( dev->antialized ) {
#ifdef HAVE_AGG
    dev->m_buffer->SetRGB( x, y, GetRValue(colour), GetGValue(colour), GetBValue(colour) );    
#endif
  } else  {
    const wxPen oldpen=dev->dc->GetPen();
    dev->dc->SetPen( *(wxThePenList->FindOrCreatePen(wxColour(GetRValue(colour), GetGValue(colour), GetBValue(colour)),
                                                   1, wxSOLID)) );
    dev->dc->DrawPoint( x, y );
    dev->dc->SetPen( oldpen );
  }

  if( !(dev->resizing) && dev->ownGUI ) {
		AddtoClipRegion( dev, x, y, x+1, y+1 );
    dev->comcount++;
		if( dev->comcount>MAX_COMCOUNT ) {
			wxRunApp( pls, true );
			dev->comcount=0;
		}
	}
}


/*--------------------------------------------------------------------------*\
 *  void plD_read_pixel_wxwidgets (PLStream *pls, short x, short y)
 *
 *  callback function, of type "plD_pixel_fp", which specifies how a single
 *  pixel is read.
\*--------------------------------------------------------------------------*/
static PLINT plD_read_pixel_wxwidgets ( PLStream *pls, short x, short y )
{
  Log_Verbose( "plD_read_pixel_wxwidgets" );

  wxPLdev *dev=(wxPLdev*)pls->dev;
  PLINT colour;
  
  if( !(dev->ready) )
    install_buffer( pls );

  if( dev->antialized ) {
#ifdef HAVE_AGG
    colour = RGB( dev->m_buffer->GetRed( x, y ), dev->m_buffer->GetGreen( x, y ),
                  dev->m_buffer->GetBlue( x, y ) );    
#endif
  } else {  
#ifdef __WXGTK__
    // The GetPixel method is incredible slow for wxGTK. Therefore we set the colour
    // always to the background color, since this is the case anyway 99% of the time.
    PLINT bgr, bgg, bgb;  /* red, green, blue */
    plgcolbg( &bgr, &bgg, &bgb );  /* get background color information */
    colour=RGB( bgr, bgg, bgb );
#else
    wxColour col;
    dev->dc->GetPixel( x, y, &col );
    colour = RGB( col.Red(), col.Green(), col.Blue());
#endif
	}

  return colour;
}


/*----------------------------------------------------------------------*\
 *  void init_freetype_lv1 (PLStream *pls)
 *
 *  "level 1" initialisation of the freetype library.
 *  "Level 1" initialisation calls plD_FreeType_init(pls) which allocates
 *  memory to the pls->FT structure, then sets up the pixel callback
 *  function.
\*----------------------------------------------------------------------*/
static void init_freetype_lv1( PLStream *pls )
{
  Log_Verbose( "init_freetype_lv1" );

  wxPLdev *dev=(wxPLdev*)pls->dev;

	plD_FreeType_init( pls );

	FT_Data *FT=(FT_Data *)pls->FT;
	FT->pixel = (plD_pixel_fp)plD_pixel_wxwidgets;

  /*
   *  See if we have a 24 bit device (or better), in which case
   *  we can use the better antialising.
   */
  if( dev->antialized ) {
    FT->BLENDED_ANTIALIASING=1;
    FT->read_pixel= (plD_read_pixel_fp)plD_read_pixel_wxwidgets;
    FT->set_pixel= (plD_set_pixel_fp)plD_set_pixel_wxwidgets;
  } else {
    // the bitmap we are using in the antialized case has always
    // 32 bit depth
    FT->BLENDED_ANTIALIASING=1;
    FT->read_pixel= (plD_read_pixel_fp)plD_read_pixel_wxwidgets;
    FT->set_pixel= (plD_set_pixel_fp)plD_set_pixel_wxwidgets;
  }
}


/*----------------------------------------------------------------------*\
 *  void init_freetype_lv2 (PLStream *pls)
 *
 *  "Level 2" initialisation of the freetype library.
 *  "Level 2" fills in a few setting that aren't public until after the
 *  graphics sub-syetm has been initialised.
 *  The "level 2" initialisation fills in a few things that are defined
 *  later in the initialisation process for the GD driver.
 *
 *  FT->scale is a scaling factor to convert co-ordinates. This is used by
 *  the GD and other drivers to scale back a larger virtual page and this
 *  eliminate the "hidden line removal bug". Set it to 1 if your device
 *  doesn't have scaling.
 *
 *  Some coordinate systems have zero on the bottom, others have zero on
 *  the top. Freetype does it one way, and most everything else does it the
 *  other. To make sure everything is working ok, we have to "flip" the
 *  coordinates, and to do this we need to know how big in the Y dimension
 *  the page is, and whether we have to invert the page or leave it alone.
 *
 *  FT->ymax specifies the size of the page FT->invert_y=1 tells us to
 *  invert the y-coordinates, FT->invert_y=0 will not invert the
 *  coordinates.
\*----------------------------------------------------------------------*/

static void init_freetype_lv2( PLStream *pls )
{
  Log_Verbose( "init_freetype_lv2" );

  wxPLdev *dev=(wxPLdev *)pls->dev;
  FT_Data *FT=(FT_Data *)pls->FT;
  
  FT->scalex=dev->scalex;
  FT->scaley=dev->scaley;
  FT->ymax=dev->height;
  FT->invert_y=1;
  FT->smooth_text=0;
  
  if ((FT->want_smooth_text==1)&&(FT->BLENDED_ANTIALIASING==0))    /* do we want to at least *try* for smoothing ? */
  {
    FT->ncol0_org=pls->ncol0;                                   /* save a copy of the original size of ncol0 */
    FT->ncol0_xtra=16777216-(pls->ncol1+pls->ncol0);            /* work out how many free slots we have */
    FT->ncol0_width=FT->ncol0_xtra/(pls->ncol0-1);              /* find out how many different shades of anti-aliasing we can do */
    if (FT->ncol0_width>4)     /* are there enough colour slots free for text smoothing ? */
       {
        if (FT->ncol0_width>max_number_of_grey_levels_used_in_text_smoothing)
           FT->ncol0_width=max_number_of_grey_levels_used_in_text_smoothing;                 /* set a maximum number of shades */
        plscmap0n(FT->ncol0_org+(FT->ncol0_width*pls->ncol0));      /* redefine the size of cmap0 */
    /* the level manipulations are to turn off the plP_state(PLSTATE_CMAP0)
    * call in plscmap0 which (a) leads to segfaults since the GD image is
    * not defined at this point and (b) would be inefficient in any case since
    * setcmap is always called later (see plD_bop_png) to update the driver
    * color palette to be consistent with cmap0. */
         {
          PLINT level_save;
          level_save = pls->level;
          pls->level = 0;
          pl_set_extended_cmap0(pls, FT->ncol0_width, FT->ncol0_org); /* call the function to add the extra cmap0 entries and calculate stuff */
          pls->level = level_save;
         }
        FT->smooth_text=1;      /* Yippee ! We had success setting up the extended cmap0 */
      }
  } else if ((FT->want_smooth_text==1)&&(FT->BLENDED_ANTIALIASING==1))    /* If we have a truecolour device, we wont even bother trying to change the palette */
   {
     FT->smooth_text=1;
   } else
     plwarn("Insufficient colour slots available in CMAP0 to do text smoothing.");
}

#endif


/*--------------------------------------------------------------------------*\
 * GetCursorCmd()
 *
 * Waits for a graphics input event and returns coordinates.
\*--------------------------------------------------------------------------*/
static void GetCursorCmd( PLStream* pls, PLGraphicsIn* ptr)
{
  Log_Verbose( "GetCursorCmd" );

  wxPLdev *dev=(wxPLdev *)pls->dev;
  PLGraphicsIn *gin = &(dev->gin);

  /* Initialize */
  plGinInit( gin );
  dev->locate_mode = LOCATE_INVOKED_VIA_API;  
  dev->draw_xhair=true;

  /* Run event loop until a point is selected */
  wxRunApp( pls, false );
  
  *ptr = *gin;
  if( dev->locate_mode ) {
    dev->locate_mode = 0;
    dev->draw_xhair=false;
  }
}




/*----------------------------------------------------------------------*\
 *  This part includes wxWidgets specific functions, which allow to
 *  open a window from the command line, if needed.
\*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*\
 *  void install_buffer( PLStream *pls )
 *
 *  If this driver is called from a command line executable (and not
 *  from within a wxWidgets program), this function prepares a DC and a
 *  bitmap to plot into.
\*----------------------------------------------------------------------*/
static void install_buffer( PLStream *pls )
{
  Log_Verbose( "install_buffer" );

  wxPLdev* dev = (wxPLdev*)pls->dev;
  static bool initApp=false;
  
  if( !initApp ) {
    /* this hack enables to have a GUI on Mac OSX even if the 
       program was called from the command line (and isn't a bundle) */
#ifdef __WXMAC__
    ProcessSerialNumber psn;

    GetCurrentProcess( &psn );
    CPSEnableForegroundOperation( &psn );
    SetFrontProcess( &psn );
#endif

    /* initialize wxWidgets */
    wxInitialize();
    wxLog::GetActiveTarget();
    wxTRY {
      wxGetApp().CallOnInit();
    }
    wxCATCH_ALL( wxGetApp().OnUnhandledException(); fprintf(stderr, "Can't init wxWidgets!\n"); exit(0); )
    initApp=true;
  }
  
  dev->m_frame = new wxPLplotFrame( wxT("wxWidgets PLplot App"), pls );
  wxGetApp().AddFrame( dev->m_frame );
  dev->m_frame->SetClientSize( dev->width, dev->height );
  dev->m_frame->Show( true );
  dev->m_frame->Raise();
	
  dev->bm_width=1024;
  dev->bm_height=800;
  /* get a DC and a bitmap or an imagebuffer */
  if( dev->antialized ) {
    /* get a new wxImage (image buffer) */
    dev->m_buffer = new wxImage( dev->bm_width, dev->bm_height );
#ifdef HAVE_AGG  
    dev->m_rendering_buffer = new agg::rendering_buffer;
    dev->m_rendering_buffer->attach( dev->m_buffer->GetData(), dev->bm_width, dev->bm_height, dev->bm_width*3 );
#endif
  } else {
    if( !(dev->dc) )
      dev->dc = new wxMemoryDC();
    
    /* get a new bitmap */
    dev->m_bitmap = new wxBitmap( dev->bm_width, dev->bm_height, 32 );
    ((wxMemoryDC*)dev->dc)->SelectObject( wxNullBitmap );   /* deselect bitmap */
    ((wxMemoryDC*)dev->dc)->SelectObject( *(dev->m_bitmap) );   /* select new bitmap */
  }
  
  dev->ready = true;
  dev->ownGUI = true;

  /* replay command we may have missed */
  plD_bop_wxwidgets( pls );
}
 

/*----------------------------------------------------------------------*\
 *  void wxRunApp( PLStream *pls, bool runonce )
 *
 *  This is a hacked wxEntry-function, so that wxUninitialize is not
 *  called twice. Here we actually start the wxApplication.
\*----------------------------------------------------------------------*/
static void wxRunApp( PLStream *pls, bool runonce )
{
  Log_Verbose( "wxRunApp" );  
  
  wxPLdev* dev = (wxPLdev*)pls->dev;
  
  dev->waiting=true;
  wxTRY
  {
    class CallOnExit
    {
    public:
      /* only call OnExit if exit is true (i.e. due an exception) */
      ~CallOnExit() { if(exit) wxGetApp().OnExit(); }
      bool exit;
    } callOnExit;
    
    callOnExit.exit=true;
    wxGetApp().SetAdvanceFlag( runonce );
    wxGetApp().SetRefreshFlag();

		/* add an idle event is necessary for Linux (wxGTK2)
		   but not for Windows, but it doesn't harm */
	  wxIdleEvent event;
    wxGetApp().AddPendingEvent( event );
		wxGetApp().OnRun();   /* start wxWidgets application     */
    callOnExit.exit=false;
  }
  wxCATCH_ALL( wxGetApp().OnUnhandledException(); fprintf(stderr, "Problem running wxWidgets!\n"); exit(0); )

  if( dev->exit ) {
    wxGetApp().OnExit();
    plexit("");
  }

  dev->waiting=false;
}


/*----------------------------------------------------------------------*\
 *  bool wxPLplotApp::OnInit()
 *
 *  This method is called before the applications gets control. A frame
 *  is created here.
\*----------------------------------------------------------------------*/
bool wxPLplotApp::OnInit()
{
  Log_Verbose( "wxPLplotApp::OnInit" );
  
  exit=false;
  advance=false;

  return true;
}


/*----------------------------------------------------------------------*\
 *  void wxPLplotApp::SetRefreshFlag( bool flag )
 *
 *  XXX - missing
\*----------------------------------------------------------------------*/
void wxPLplotApp::SetRefreshFlag( bool flag )
{
  Log_Verbose( "wxPLplotApp::SetRefreshFlag" );

	for( size_t i=0; i<FrameArray.GetCount(); i++)
		FrameArray[i]->SetRefreshFlag( flag );
}


/*----------------------------------------------------------------------*\
 *  void wxPLplotApp::OnIdle( wxIdleEvent& WXUNUSED(event) )
 *
 *  XXX - missing
\*----------------------------------------------------------------------*/
void wxPLplotApp::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
  Log_Verbose( "wxPLplotApp::OnIdle" );

	bool refresh=false;

  if( exit )
    ExitMainLoop();

	for( size_t i=0; i<FrameArray.GetCount(); i++)
		refresh |= FrameArray[i]->GetRefreshFlag();

  if( advance && !refresh )
    ExitMainLoop();
}


/*----------------------------------------------------------------------*\
 *  wxPLplotFrame::wxPLplotFrame( const wxString& title, PLStream *pls )
 *
 *  Constructor of wxPLplotFrame, where we create the menu and add the
 *  wxPLplotWindow. We need also to know the current PLStream.
\*----------------------------------------------------------------------*/
wxPLplotFrame::wxPLplotFrame( const wxString& title, PLStream *pls )
             : wxFrame( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                        wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION |
                        wxCLOSE_BOX | wxRESIZE_BORDER | wxCLIP_CHILDREN ) 
{
  Log_Verbose( "wxPLplotFrame::wxPLplotFrame" );
  m_dev=(wxPLdev*)pls->dev;

  m_panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN );
  wxBoxSizer* box = new wxBoxSizer( wxVERTICAL );
	m_window = new wxPLplotWindow( m_panel, pls );
  box->Add( m_window, 1, wxALL | wxEXPAND, 0 );
  m_panel->SetSizer( box );
	m_window->SetFocus();
	  
  wxMenu* saveMenu = new wxMenu;
  for( size_t j=0; j<sizeof(dev_entries)/sizeof(dev_entry); j++ )
    for( size_t i=0; i<m_dev->ndev; i++ ) {
      if( !strcmp(m_dev->devName[i], dev_entries[j].dev_name.mb_str()) )
        saveMenu->Append( wxPL_Save+j, dev_entries[j].dev_menu_short, dev_entries[j].dev_menu_long );
    }

  wxMenu* fileMenu = new wxMenu;
#if (wxMAJOR_VERSION<=2) & (wxMINOR_VERSION<=6)
  fileMenu->Append( -1, wxT("Save plot as..."), saveMenu, wxT("Save this plot as ...!") );
#else
  fileMenu->AppendSubMenu( saveMenu, wxT("Save plot as..."), wxT("Save this plot as ...!") );
#endif
  fileMenu->Append( wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Exit wxWidgets PLplot App") );

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append( fileMenu, wxT("&File") );
  SetMenuBar( menuBar );

  SetTitle( wxT("wxWidgets PLplot App") );
  SetIcon( wxIcon(graph) );
}


/*----------------------------------------------------------------------*\
 *  void wxPLplotFrame::OnMenu( wxCommandEvent& event )
 *
 *  Event method, which is called if user 
\*----------------------------------------------------------------------*/
void wxPLplotFrame::OnMenu( wxCommandEvent& event )
{
  Log_Verbose( "wxPLplotFrame::OnMenu" );

  switch( event.GetId() )
  {
  case wxID_EXIT:
		{
			wxMessageDialog dialog( this, wxT("Do you really want to quit?"), wxT("Close wxWidgets PLplot App?"), wxYES_NO |wxNO_DEFAULT| wxICON_EXCLAMATION );
			if( dialog.ShowModal() == wxID_YES ) {
				m_dev->exit=true;
				wxGetApp().ExitMainLoop();
			}
		}
		break;
  }
    
  size_t index=event.GetId()-wxPL_Save;
  if( (index>=0) && (index<sizeof(dev_entries)/sizeof(dev_entry)) ) {
    wxFileDialog dialog( this, wxT("Save plot as ")+dev_entries[index].dev_name, wxT(""), wxT(""),
                         dev_entries[index].dev_file_app+wxT("|All Files (*.*)|*.*"),
#if (wxMAJOR_VERSION<=2) & (wxMINOR_VERSION<=6)
                         wxSAVE | wxOVERWRITE_PROMPT );
#else
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
#endif	
    if (dialog.ShowModal() == wxID_OK) {
      const wxCharBuffer buf1=dialog.GetPath().mb_str();
      const wxCharBuffer buf2=dev_entries[index].dev_name.mb_str();
      SavePlot( (const char*)buf1, (const char*)buf2, 800, 600 );
    }
  }
}

/*----------------------------------------------------------------------*\
 *  void wxPLplotFrame::OnClose( wxCloseEvent& event )
 *
 *  Event method, which is called if user 
\*----------------------------------------------------------------------*/
void wxPLplotFrame::OnClose( wxCloseEvent& event )
{
  Log_Verbose( "wxPLplotFrame::OnClose" );

  // Are you old enough to know that you want really to quit?
  wxMessageDialog dialog( this, wxT("Do you really want to quit?"), wxT("Close wxWidgets PLplot App?"), wxYES_NO |wxNO_DEFAULT| wxICON_EXCLAMATION );
  if( dialog.ShowModal() == wxID_YES ) {
		m_dev->exit=true;
		wxGetApp().ExitMainLoop();
	}
}


/*----------------------------------------------------------------------*\
 *  bool wxPLplotFrame::SavePlot( const char* filename, cost char* dev, int width,
 *                                int height )
 *
 *  This function saves the current plot to a file (filename) using a
 *  device (devname) with given width and height. There is no test if
 *  the device really exists.
\*----------------------------------------------------------------------*/
bool wxPLplotFrame::SavePlot( const char* filename, const char* devname, int width,  int height )
{  
  int pls, pls_save;
  FILE *sfile;

	if( (sfile = fopen(filename, "wb+")) == NULL) {
    if( m_dev->ownGUI ) {
      wxMessageDialog dialog( 0, wxT("Couldn't open file for saving!"), wxT("plPlot error"),
														  wxOK|wxICON_ERROR );
      dialog.ShowModal();
    } 
		return false;
	}

	plgstrm( &pls );
	plmkstrm( &pls_save );  
	if( pls_save<0 ) {
    if( m_dev->ownGUI ) {
      wxMessageDialog dialog( 0, wxT("Couldn't open file for saving!"), wxT("plPlot error"),
																 wxOK|wxICON_ERROR );
      dialog.ShowModal();
    } 
		return false;
	}
	plsdev( devname );
  //plsfile( sfile );
  plsfnam(filename);       /* file name */
  
  //plspage( 0., 0., width, height, 0, 0 );
	plcpstrm( pls, 0);
	pladv( 0 );
	plreplot();
  plend1();
	plsstrm( pls );

  return true;
}


/*----------------------------------------------------------------------*\
 *  wxPLplotWindow::wxPLplotWindow( const wxString& title )
 *
 *  Constructor of wxPLplotFrame, where we create the menu.
\*----------------------------------------------------------------------*/
wxPLplotWindow::wxPLplotWindow( wxWindow* parent, PLStream *pls )
             : wxWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                         wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN )
{
  Log_Verbose( "wxPLplotWindow::wxPLplotWindow" );

  m_pls=pls;
  m_dev=(wxPLdev*)pls->dev;
	refresh=false;
  mouse_x=old_mouse_x=-1;
  mouse_y=old_mouse_y=-1;
  xhair_drawn=false;

	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
}


/*----------------------------------------------------------------------*\
 *  void wxPLplotWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
 *
 *  Event method where the plots are actually drawn. Since the plots
 *  are already drawn into bitmaps, which just copy them into to client
 *  area. This method is also called, if (part) of the client area was
 *  invalidated and a refresh is necessary.
\*----------------------------------------------------------------------*/
void wxPLplotWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  Log_Verbose( "wxPLplotWindow::OnPaint" );
	//static int counter=0;  
  /* copy bitmap into client area */
  wxPaintDC dc( this );

  /* only update damaged regions */
  int vX, vY, vW, vH; 
  wxRegionIterator upd( GetUpdateRegion() ); 

  if( m_dev->draw_xhair && upd && xhair_drawn ) {
    dc.SetLogicalFunction( wxINVERT );
    dc.CrossHair( mouse_x, mouse_y );
    dc.SetLogicalFunction( wxCOPY );
    xhair_drawn = false;
  }

  while( upd ) {
    vX = upd.GetX();
    vY = upd.GetY();
    vW = upd.GetW();
    vH = upd.GetH();

		//printf( "Clipping region: x=%d, y=%d, width=%d, height=%d, counter=%d\n", vX, vY, vW, vH, counter++ );
  
    if( m_dev->antialized ) {
    	if( m_dev->m_buffer ) {
	      wxMemoryDC MemoryDC;
				wxBitmap bitmap( m_dev->m_buffer->GetSubImage(wxRect(vX, vY, vW, vH)), -1 );
	      MemoryDC.SelectObject( bitmap );
	      dc.Blit( vX, vY, vW, vH, &MemoryDC, 0, 0 );
	      MemoryDC.SelectObject( wxNullBitmap );
    	}
    }
    else {
    	if( m_dev->dc )
   	   dc.Blit( vX, vY, vW, vH, m_dev->dc, vX, vY );
    }

    upd ++ ;
  }

  if( m_dev->draw_xhair && !xhair_drawn ) {
    dc.SetLogicalFunction( wxINVERT );
    dc.CrossHair( mouse_x, mouse_y );
    dc.SetLogicalFunction( wxCOPY );
    xhair_drawn=true;
  }  
}
 

void wxPLplotWindow::OnChar( wxKeyEvent& event )
{
  Log_Verbose( "wxPLplotWindow::OnChar" );

  int keycode = event.GetKeyCode();
  switch( keycode ) {
    case 'L':
      m_dev->locate_mode = LOCATE_INVOKED_VIA_DRIVER;  
      m_dev->draw_xhair=true;
      DrawCrosshair();
      break;
    case 'Q':
    case WXK_ESCAPE:
      m_dev->exit=true;
      wxGetApp().SetExitFlag();
      break;
    case WXK_RETURN:
    case WXK_SPACE:
    case WXK_RIGHT:
      wxGetApp().SetAdvanceFlag();
      break;
    default:
      break;      
  }
  
  event.Skip();
}

void wxPLplotWindow::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
  Log_Verbose( "wxPLplotWindow::OnIdle" );

  if( refresh ) {
    if(!m_dev->newclipregion) {
      static wxRect rect;
			rect.x=m_dev->clipminx;
			rect.y=m_dev->clipminy;
			rect.width=m_dev->clipmaxx-m_dev->clipminx+1;
			rect.height=m_dev->clipmaxy-m_dev->clipminy+1;
#if (wxMAJOR_VERSION<=2) & (wxMINOR_VERSION<=5)
      RefreshRect( rect ); 
#else			
      RefreshRect( rect, false );  /* don't erase background */
#endif
      m_dev->newclipregion=true;
			m_dev->clipminx=m_dev->width;
			m_dev->clipmaxx=0;
			m_dev->clipminy=m_dev->height;
			m_dev->clipmaxy=0;
    } else
      Refresh( false );
    refresh=false;
  }
}


void wxPLplotWindow::OnErase( wxEraseEvent &WXUNUSED(event) )
{  
  Log_Verbose( "wxPLplotWindow::OnErase" );
}


void wxPLplotWindow::OnSize( wxSizeEvent & WXUNUSED(event) )
{
  Log_Verbose( "wxPLplotWindow::OnSize" );

  int width, height;
  GetClientSize( &width, &height );
  
  if( m_dev->waiting ) {
    if( (width!=m_dev->width) || (height!=m_dev->height) ) {
      /* get a new bitmap if new size is bigger as bitmap size */
      if( (width>m_dev->bm_width) || (height>m_dev->bm_height) ) {
				m_dev->bm_width = m_dev->bm_width > width ? m_dev->bm_width : width;
				m_dev->bm_height = m_dev->bm_height > height ? m_dev->bm_height : height;
        if( m_dev->antialized ) {
          /* get a new wxImage (image buffer) */
          if( m_dev->m_buffer )
            delete m_dev->m_buffer;
          m_dev->m_buffer = new wxImage( m_dev->bm_width, m_dev->bm_height );
        } else {
          ((wxMemoryDC*)m_dev->dc)->SelectObject( wxNullBitmap );   /* deselect bitmap */
          if( m_dev->m_bitmap )
            delete m_dev->m_bitmap;
          m_dev->m_bitmap = new wxBitmap( m_dev->bm_width, m_dev->bm_height, 32 );
          ((wxMemoryDC*)m_dev->dc)->SelectObject( *(m_dev->m_bitmap) );   /* select new bitmap */
        }
      }

      wx_set_size( m_pls, width, height );
      m_dev->resizing = true;
      plRemakePlot( m_pls );
      m_dev->resizing = false;
      Refresh();
    }
  }
}


void wxPLplotWindow::OnMaximize( wxMaximizeEvent & WXUNUSED(event) )
{
  Log_Verbose( "wxPLplotWindow::OnMax" );

  wxSizeEvent event( GetClientSize() );
  AddPendingEvent( event );
}

void wxPLplotWindow::OnMouse( wxMouseEvent &event )
{
  Log_Verbose( "wxPLplotWindow::OnMouse" );

  PLGraphicsIn *gin = &(m_dev->gin);
  wxPoint pos( event.GetPosition() );

  mouse_x = pos.x;
  mouse_y = pos.y;
  
  if( event.LeftDown() ) {    
    int width, height;
    GetClientSize( &width, &height );

    gin->pX = pos.x;
    gin->pY = pos.y;
    gin->dX = (PLFLT) pos.x / (width - 1);
    gin->dY = 1.0 - (PLFLT) pos.y / (height - 1);

    gin->button = 1;  // X11/X.h: #define Button1	1
    gin->state = 1<<8;  // X11/X.h: #define Button1Mask	(1<<8)
    gin->keysym = 0x20;  // TODO: ????? from xwin.c
  }

  if( event.LeftDown() ) {    
  	if( plTranslateCursor(gin) ) {
      /* If invoked by the API, we're done */
      /* Otherwise send report to stdout */
	    if( m_dev->locate_mode == LOCATE_INVOKED_VIA_DRIVER )
		    printf( "%f %f\n", gin->wX, gin->wY );
    } else {
      /* Selected point is out of bounds, so end locate mode */
        m_dev->locate_mode = 0;
        m_dev->draw_xhair=false;
    }
  }

  DrawCrosshair();
}

void wxPLplotWindow::DrawCrosshair()
{
  /* draw cross hair */
  wxClientDC dc(this);
  if( m_dev->draw_xhair ) {
    if( (mouse_x!=old_mouse_x) || (mouse_y!=old_mouse_y) ) {
      dc.SetLogicalFunction( wxINVERT );
      if( xhair_drawn )
        dc.CrossHair( old_mouse_x, old_mouse_y );
      dc.CrossHair( mouse_x, mouse_y );
      dc.SetLogicalFunction( wxCOPY );
      old_mouse_x=mouse_x;
      old_mouse_y=mouse_y;
      xhair_drawn=true;  
    }
  } else {
    if( xhair_drawn ) {
      dc.SetLogicalFunction( wxINVERT );
      dc.CrossHair( old_mouse_x, old_mouse_y );
      dc.SetLogicalFunction( wxCOPY );
      xhair_drawn=false;
      old_mouse_x=old_mouse_y=-1;
    }
  }  
}

#else
int pldummy_wxwidgets()
{
    return 0;
}

#endif				/* PLD_wxwidgets */
