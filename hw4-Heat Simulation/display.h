/* Initial heat distrubution program */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "models.h"
#include "const.h"

Window          win;                            /* initialization for a window */
unsigned
int             width, height,                  /* window size */
		border_width,                   /*border width in pixels */
		idth, display_height,  /* size of screen */
		screen;                         /* which screen */

char            *window_name = "Temperature Simulation", *display_name = NULL;
GC              gc;
unsigned
long            valuemask = 0;
XGCValues       values;
Display         *display;
XSizeHints      size_hints;
Pixmap          bitmap;
FILE            *fp, *fopen ();	
Colormap	default_cmap;
XColor		color[256];

int temperatue_to_color_pixel(double t)
{
	return color[(int)(t/5.0f)-1].pixel;
}

void XWindow_Init(TemperatureField *field)
{    
        XSetWindowAttributes attr[1];       
       
        /* connect to Xserver */

        if (  (display = XOpenDisplay (display_name)) == NULL ) {
           fprintf (stderr, "drawon: cannot connect to X server %s\n",
                                XDisplayName (display_name) );
        exit (-1);
        }
        
        /* get screen size */

        screen = DefaultScreen (display);

        /* set window size *///XFlush (display);

        width = field->y;
	    height = field->x;


        border_width = 4;
        win = XCreateSimpleWindow (display, RootWindow (display, screen),
                                width, height, width, height, border_width, 
                                BlackPixel (display, screen), WhitePixel (display, screen));

        size_hints.flags = USPosition|USSize;
        size_hints.x = 0;
        size_hints.y = 0;
        size_hints.width = width;
        size_hints.height = height;
        size_hints.min_width = 300;
        size_hints.min_height = 300;
        
        XSetNormalHints (display, win, &size_hints);
        XStoreName(display, win, window_name);

        

        gc = XCreateGC (display, win, valuemask, &values);

	default_cmap = DefaultColormap(display, screen);
        XSetBackground (display, gc, WhitePixel (display, screen));
        XSetForeground (display, gc, BlackPixel (display, screen));
        XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

        attr[0].backing_store = Always;
        attr[0].backing_planes = 1;
        attr[0].backing_pixel = BlackPixel(display, screen);

        XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

        XMapWindow (display, win);
        XSync(display, 0); 

	
	int i;
    int red[20] = {2, 7, 27, 48, 78, 116, 155, 153, 151, 234, 250, 242, 240, 238, 231, 217, 194, 181, 138, 80};
    int green[20] = {12, 30, 68, 106, 138, 163, 188, 205, 232, 219, 204, 155, 132, 102, 75, 51, 0, 1, 5, 0};
    int blue[20] = {100, 120, 159, 199, 221, 226, 232, 208, 173, 112, 79, 0, 10, 24, 26, 18, 3, 9, 25, 15};
    
	for (i=0; i<20; ++i)
	{
	    color[i].green = green[i]*257;
	    color[i].red = red[i]*257;
	    color[i].blue = blue[i]*257;
	    color[i].flags = DoRed | DoGreen | DoBlue;
	    XAllocColor(display, default_cmap, &color[i]);
	}
}

void XResize(TemperatureField *field)
{
    XResizeWindow(display, win, field->y, field->x);
}

void XRedraw(TemperatureField *field)
{
    int i, j;
    for (i=0; i<field->x; ++i)
        for (j=0; j<field->y; ++j)
	{
		XSetForeground(display, gc, temperatue_to_color_pixel(field->t[i][j]));
	        XDrawPoint (display, win, gc, j, i);
	}
    XFlush (display);
}

