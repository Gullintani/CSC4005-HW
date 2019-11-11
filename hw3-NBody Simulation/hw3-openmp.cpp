#include <omp.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>


const int WIDTH = 800;
const int HEIGHT = 800;

const double G = 6.67259;
const int MAX_x = 500;
const int MIN_x = 300;
const int MAX_y = 500;
const int MIN_y = 300;
const int MAX_w = 1000;
const int MIN_w = 100;

int body_num = 500;
int iteration_num = 1000;
const double T = 0.01;

struct Body{
    double x,y;
    double vx,vy;
    double w;
};


Display *display;
GC gc;
Window win;

int main (int argc,char *argv[]) {
    printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 3, N-Body Simulation, OpenMP Implementation\n");
    int threads;
    body_num = atoi(argv[1]);
    iteration_num = atoi(argv[2]);
    threads = atoi(argv[3]);


    Window          win;  
    unsigned
    int             width, height,   
                    x, y,             
                    border_width, 
                    display_width, 
                    display_height,       
                    screen;  

    char            *display_name = NULL;
    GC              gc;
    unsigned
    long            valuemask = 0;
    XGCValues       values;
    Display         *display;
    XSizeHints      size_hints;

    XInitThreads();

    XSetWindowAttributes attr[1]; 

    if ((display = XOpenDisplay(display_name)) == NULL) {
       fprintf(stderr, "Something wrong with the X server %s\n",
                            XDisplayName(display_name));
       exit(-1);
    }

    screen = DefaultScreen(display);
    display_width = DisplayWidth(display, screen);
    display_height = DisplayHeight(display, screen);

    width = WIDTH;
    height = HEIGHT;

    x = 0;
    y = 0;

    border_width = 4;
    win = XCreateSimpleWindow (display, 
                            RootWindow(display, screen),
                            x, y, width, height, border_width, 
                            BlackPixel(display, screen), 
                            WhitePixel(display, screen));

    size_hints.flags = USPosition|USSize;
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = width;
    size_hints.height = height;
    size_hints.min_width = 300;
    size_hints.min_height = 300;
    
    XSetNormalHints(display, win, &size_hints);

    gc = XCreateGC(display, win, valuemask, &values);

    XSetBackground(display, gc, WhitePixel(display, screen));
    XSetForeground(display, gc, BlackPixel(display, screen));
    XSetLineAttributes(display, gc, 1, LineSolid, CapRound, 
                    JoinRound);

    attr[0].backing_store = Always;
    attr[0].backing_planes = 1;
    attr[0].backing_pixel = BlackPixel(display, screen);

    XChangeWindowAttributes(display, win, 
        CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

    XMapWindow(display, win);
    XSync(display, 0);

    struct timespec start, finish;
    double count_time;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int i,j,k;
    struct Body Nbody[body_num];
    double vx[body_num];
    double vy[body_num];
    double deltaX, deltaY;
    double distance;
    double F;
    int scr = DefaultScreen(display);
    int pm = XCreatePixmap(display,win,WIDTH,HEIGHT,DefaultDepth(display,scr));

    srand(time(NULL));
    int count[body_num], startindex[body_num];
    if (body_num % threads == 0) {
        for (int i=0;i<threads;i++) {
            count[i] = body_num / threads;
            startindex[i] = i * body_num / threads;
        }
    }
    else {
        for (int i=0;i<threads;i++) {
            count[i] = body_num / threads;
            if (i<body_num % threads){
                count[i] += 1;
            }
            if (i==0) {
                startindex[i] = 0;
            }
            else {
                startindex[i] = count[i-1] +startindex[i-1];
            }
        }
    }
    for(i=0;i<body_num;i++) {
        Nbody[i].x = rand() % (MAX_x-MIN_x)+MIN_x;
        Nbody[i].y = rand() % (MAX_y-MIN_y)+MIN_y;
        Nbody[i].vx = 0;
        Nbody[i].vy = 0;
        Nbody[i].w = rand() % (MAX_w-MIN_w)+MIN_w;
    }

    for(k=0;k<iteration_num;k++) {
        XSetForeground(display,gc,0);
        XFillRectangle(display,pm,gc,0,0,WIDTH,HEIGHT);

        #pragma omp parallel num_threads(threads)
        {
            long tid = omp_get_thread_num();
            for(i=startindex[tid];i<startindex[tid]+count[tid];i++) {
                for(j=0;j<body_num;j++) {
                    if (j==i) continue;
                    deltaX = Nbody[j].x - Nbody[i].x;
                    deltaY = Nbody[j].y - Nbody[i].y;
                    distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
                    F = G * Nbody[j].w / (distance*distance);
                    if(distance >= 25 && distance != 0 && Nbody[j].w != 0) {
                        vx[i] = vx[i] + T * F * (deltaX/distance) ;
                        vy[i] = vy[i] + T * F * (deltaY/distance) ;
                    }
                }
            }
        #pragma omp barrier
        }

        for(i=0;i<body_num;i++) {
                Nbody[i].x = Nbody[i].x + vx[i] * T;
                Nbody[i].y = Nbody[i].y + vy[i] * T;
                Nbody[i].vx = vx[i];
                Nbody[i].vy = vy[i];
        }
        XSetForeground(display, gc, WhitePixel(display,scr));
        for(i=0;i<body_num;i++) {
            if(Nbody[i].y <= HEIGHT && Nbody[i].x <= WIDTH)
            XDrawPoint(display, pm, gc, Nbody[i].y, Nbody[i].x);
        }
        XCopyArea(display,pm,win,gc,0,0,WIDTH,HEIGHT,0,0);
        XFlush(display);
    }
    // Time jobs
    clock_gettime(CLOCK_MONOTONIC, &finish);
    count_time = finish.tv_sec-start.tv_sec + (double)(finish.tv_nsec - start.tv_nsec)/1000000000.0;
    // End info
    printf("Execution Time is: %f seconds\n", count_time);
    printf("Finish Drawing...\n");
    
    XFreePixmap(display,pm);
    XCloseDisplay(display);
    return 0;
}