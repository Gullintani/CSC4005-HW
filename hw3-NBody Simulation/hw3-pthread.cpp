#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <math.h>
#include <pthread.h>
#include <time.h>


const int WIDTH = 800;
const int HEIGHT = 800;
const double G = 6.67259;
const int MAX_x = 500;
const int MIN_x  = 300;
const int MAX_y = 500;
const int MIN_y  = 300;
const int MAX_w = 500;
const int MIN_w  = 100;

int body_num = 500;
int iteration_num = 1000;
const double T = 0.01;

struct Body{
    double x,y;
    double vx,vy;
    double w;
}Nbody[505];


int i,j,k;
double vx[505];
double vy[505];


Window win;
GC gc;
Display *display;

pthread_mutex_t mutex;
int startx[2];

void *forcecal(void* para)
{	
    int *startx = (int *)para;
    int startX = startx[0];

    double deltaX, deltaY;
    double distance;
    double F;
    while(startX < body_num - 8){
		for(j=0;j<body_num;j++)
		{
		if (j==startX) continue;
		deltaX = Nbody[j].x - Nbody[startX].x;
		deltaY = Nbody[j].y - Nbody[startX].y;
		distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
		if(distance == 0) continue;
		F = G * Nbody[j].w / (distance*distance);
		if(distance >= 25)
			{
			vx[startX] = vx[startX] + T * F * deltaX/distance;
			vy[startX] = vy[startX] + T * F * deltaY/distance;
			}
		}		
    pthread_mutex_lock(&mutex);
    startX++;
    pthread_mutex_unlock(&mutex);
    }
}

int main (int argc,char *argv[])
{       
        printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 3, N-Body Simulation, Pthread Implementation\n");
        int nthread;
        body_num = atoi(argv[1]);
        iteration_num = atoi(argv[2]);
        nthread = atoi(argv[3]);

	    XInitThreads();

        Window          win;                            /* initialization for a window */
        unsigned
        int             width, height,                  /* window size */
                        x, y,                           /* window position */
                        border_width,                   /*border width in pixels */
                        display_width, display_height,  /* size of screen */
                        screen;                         /* which screen */

        char            *display_name = NULL;
        unsigned
        long            valuemask = 0;
        XGCValues       values;
        XSizeHints      size_hints;
        
        XSetWindowAttributes attr[1];
       
        /* connect to Xserver */

        if (  (display = XOpenDisplay (display_name)) == NULL ) {
           fprintf (stderr, "Something wrong with the X server %s\n",
                                XDisplayName (display_name) );
        }
        
        /* get screen size */

        screen = DefaultScreen (display);
        display_width = DisplayWidth (display, screen);
        display_height = DisplayHeight (display, screen);

        /* set window size */

        width = WIDTH;
        height = HEIGHT;

        /* set window position */

        x = 0;
        y = 0;

        /* create opaque window */

        border_width = 4;
        win = XCreateSimpleWindow (display, RootWindow (display, screen),
                                x, y, width, height, border_width, 
                                BlackPixel (display, screen), WhitePixel (display, screen));

        size_hints.flags = USPosition|USSize;
        size_hints.x = x;
        size_hints.y = y;
        size_hints.width = width;
        size_hints.height = height;
        size_hints.min_width = 300;
        size_hints.min_height = 300;
        
        XSetNormalHints (display, win, &size_hints);

        /* create graphics context */

        gc = XCreateGC (display, win, valuemask, &values);

        XSetBackground (display, gc, WhitePixel (display, screen));
        XSetForeground (display, gc, BlackPixel (display, screen));
        XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

        attr[0].backing_store = Always;
        attr[0].backing_planes = 1;
        attr[0].backing_pixel = BlackPixel(display, screen);

        XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

        XMapWindow (display, win);
        XSync(display, 0);

        int scr = DefaultScreen(display);
		int pm = XCreatePixmap(display,win,WIDTH,HEIGHT,DefaultDepth(display,scr));
        srand(time(NULL));
        for(i=0;i<body_num;i++)
        {
            Nbody[i].x = rand() % (MAX_x-MIN_x)+MIN_x;
            Nbody[i].y = rand() % (MAX_y-MIN_y)+MIN_y;
            Nbody[i].vx = 0;
            Nbody[i].vy = 0;
            Nbody[i].w = rand() % (MAX_w-MIN_w)+MIN_w;
        }

        // Time counting
        struct timespec start, finish;
        double time;
        // Time count start
        clock_gettime(CLOCK_MONOTONIC, &start);

        pthread_t tid[nthread];
        pthread_mutex_init(&mutex, NULL);

        for(k = 0; k < iteration_num; k++)
        {
            XSetForeground(display,gc,0);
            XFillRectangle(display,pm,gc,0,0,WIDTH,HEIGHT);
          
            for (i = 0; i < nthread; i++)
            {
                startx[0] = i;
                pthread_create(&tid[i], NULL, &forcecal, &startx);
            }

            for (i = 0; i < nthread; i++) 
            {
                pthread_join(tid[i], NULL);
            }

            for(i = 0; i < body_num; i++)
            {
                Nbody[i].x = Nbody[i].x + vx[i] * T;
                Nbody[i].y = Nbody[i].y + vy[i] * T;
                Nbody[i].vx = vx[i];
                Nbody[i].vy = vy[i];
            }

            XSetForeground(display, gc, WhitePixel(display,scr));

            for(i = 0; i < body_num - 8; i++)
            {
                if(Nbody[i].y <= HEIGHT && Nbody[i].x <= WIDTH)         
                    XDrawPoint(display, pm, gc, (int)Nbody[i].y, (int)Nbody[i].x);            
            }
            XCopyArea(display,pm,win,gc,0,0,WIDTH,HEIGHT,0,0);         
            XFlush(display);
        }
    // Time jobs
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time = finish.tv_sec-start.tv_sec + (double)(finish.tv_nsec - start.tv_nsec)/1000000000.0;
    // End info
    printf("Execution Time is: %f seconds\n", time);
    printf("Finish Drawing...\n");


    XFreePixmap(display,pm);
    XCloseDisplay(display);
	return 0;
}
