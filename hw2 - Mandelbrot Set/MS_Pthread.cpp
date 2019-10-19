#include "pthread.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <malloc.h>
#include <time.h>
#include <iostream>
#define ROOT 0
#define MAX_THREADS 10000
using namespace std;

GC gc;
Display *display;
Window window;
int screen;
XGCValues values; //Put in initGraph?
long valuemask = 0; //Put in initGraph?

double x1 = 2, x2 = -2, y1 = 2, y2 = -2; 
int nthread, width, height, MAX_ITERATION;
int enable = 0;

int tasks[MAX_THREADS];
int taskRemaining;
pthread_mutex_t	mutexJob, mutexX;

// Struct for mandelbrot calculation
struct Compl{
	double real, imag;
};

// Function declearation
void initGraph(int x, int y, int width, int height);
void* Mandelbrot_calc(void *t);

int main(int argc, char* argv[])
{
    // Read para
    nthread = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);
    MAX_ITERATION = atoi(argv[4]);
	if(strcmp("enable", argv[5])==0) enable = 1;
    
    // Print my info
    printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 2, Mandelbrot Set, Pthread Implementation\n");

    // Check enable to draw the graph or not
    if(enable){
        initGraph((int)(x1+x2)/2, (int)(y1+y2)/2, width, height);
    }

    // Time counting
    struct timespec start, finish;
    double time;
    // Time count start
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* Pthread Initialization*/
 	pthread_mutex_init(&mutexJob, NULL);	
	pthread_mutex_init(&mutexX, NULL);	
    pthread_t *threads=(pthread_t*)malloc(sizeof(pthread_t)*nthread);

    /* Pthread Creation */
    taskRemaining=width-nthread;
    for (int i=0;i<nthread;i++){
        tasks[i]=width-i-1;
        pthread_create(&threads[i],NULL,Mandelbrot_calc,(void*)&tasks[i]);
    }

    for (int i=0;i<nthread;++i){
        pthread_join(threads[i],NULL);
    }

    if (enable){
        XFlush(display);
    } 
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time = finish.tv_sec-start.tv_sec + (double)(finish.tv_nsec - start.tv_nsec)/1000000000.0;
	printf("Execution Time is: %f seconds\n", time);
    printf("displaying graph for 5 seconds.\n");
    sleep(5);
    pthread_exit(NULL);
	return 0;
}

void initGraph(int x, int y, int width, int height){
	display = XOpenDisplay(NULL);
	if(display == NULL){
		fprintf(stderr, "Opps, something wrong with display\n");
		exit(1);
	}
	screen = DefaultScreen(display);

	int border_width = 0;
	window = XCreateSimpleWindow(display, RootWindow(display, screen), x, y, width, height, border_width,
					BlackPixel(display, screen), WhitePixel(display, screen));	

	XGCValues values;
	long valuemask = 0;
	
	gc = XCreateGC(display, window, valuemask, &values);
	XSetForeground(display, gc, BlackPixel (display, screen));
	XSetBackground(display, gc, 0X0000FF00);
	XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);
	
	XMapWindow(display, window);
	XSync(display, 0);

	XSetForeground(display,gc,BlackPixel(display,screen));
	XFillRectangle(display,window,gc,0,0,width,height);
	XFlush(display);
}

void* Mandelbrot_calc(void *t){
    int pid,j,k;
    /* Mandlebrot variables */
    Compl z, c;
    float lengthsq, temp;
    pid = *((int*)t);

    while (true){
        for (j=0;j<height;j++){
            z.real = 0.0;
            z.imag = 0.0;
            double scaleX = width / (x1 - x2);
            double scaleY = height / (x1 - y2);

            c.imag = ((double)pid + scaleX * x2) / scaleX;             
            c.real = ((double)j + scaleY * y2) / scaleY; 
            k = 0;
            lengthsq=0.0;
            while (k<MAX_ITERATION && lengthsq<4.0){
                temp = z.real*z.real - z.imag*z.imag + c.real;
                z.imag = 2.0*z.real*z.imag + c.imag;
                z.real = temp;
                lengthsq = z.real*z.real+z.imag*z.imag;
                k++;
            }
            /* Draw Points */
            if (enable){
                pthread_mutex_lock(&mutexX);
                XSetForeground (display, gc, 1024 * 1024 * (k % 256));	
                XDrawPoint(display,window,gc,j,pid);
                pthread_mutex_unlock(&mutexX);
            }
        }
        pthread_mutex_lock(&mutexJob);
        if (taskRemaining<0){
            pthread_mutex_unlock(&mutexJob);
            pthread_exit(NULL);
        }
        else pid=taskRemaining--;
        pthread_mutex_unlock(&mutexJob);
    }
}
