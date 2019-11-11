#include <mpi.h>
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
const int MAX_x = 600;
const int MIN_x  = 200;
const int MAX_y = 600;
const int MIN_y  = 200;
const int MAX_w = 500;
const int MIN_w  = 100;

int body_num = 500;
int iteration_num = 1000;
const double T = 0.01;

struct Body{
    double x,y;
    double vx,vy;
    double w;
};



int main (int argc, char *argv[]) 
{
    body_num = atoi(argv[1]);
    iteration_num = atoi(argv[2]);

    int i, j, k;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double startTime, endTime;
    startTime = MPI_Wtime();

    // Create and Commit new mpi datatype MPIBody
    MPI_Datatype MPIBody;
    MPI_Type_contiguous(5, MPI_DOUBLE, &MPIBody);
    MPI_Type_commit(&MPIBody);

    MPI_Status status;
    int job = body_num /size;

    double vx[body_num];
    double vy[body_num];
    double deltaX, deltaY;
    double distance;
    double F;

    struct Body *local_Nbody;
    local_Nbody = (struct Body*)malloc(job * sizeof(struct Body));
    struct Body* Nbody = (struct Body*)malloc(body_num * sizeof(struct Body));

    if (rank == 0)
    {
        printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 3, N-Body Simulation, MPI Implementation\n");
        Window          win;                           
        unsigned
        int             width, height,                 
                        x, y,                      
                        border_width,                   
                        display_width, display_height,  
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
       
        if (  (display = XOpenDisplay (display_name)) == NULL ) 
        {
            fprintf (stderr, "Something wrong with the X server %s\n",
                                XDisplayName (display_name) );
        }

        screen = DefaultScreen (display);
        display_width = DisplayWidth (display, screen);
        display_height = DisplayHeight (display, screen);

        width = WIDTH;
        height = HEIGHT;

        x = 0;
        y = 0;

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
              Nbody[i].x = rand() % (MAX_x-MIN_x) + MIN_x;
              Nbody[i].y = rand() % (MAX_y-MIN_y) + MIN_y;
              Nbody[i].vx = 0;
              Nbody[i].vy = 0;
              Nbody[i].w = rand()% (MAX_w-MIN_w) + MIN_w;
        }

        for (i = 1; i < size; i++)
            MPI_Send(Nbody, body_num, MPIBody, i, i, MPI_COMM_WORLD);
    
        for(k=0;k<iteration_num;k++)
        {
            int startPoint = job * rank;
            XSetForeground(display,gc,0);
            XFillRectangle(display,pm,gc,0,0,WIDTH,HEIGHT);
            
            for(i=startPoint;i<job + startPoint;i++)
            {
                for(j=0;j<body_num;j++)
                {
                    if (j==i) continue;
                    deltaX = Nbody[j].x - Nbody[i].x;
                    deltaY = Nbody[j].y - Nbody[i].y;
                    distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
                    if(distance == 0) continue;
                    if(distance >= 15) {
                        F = G * Nbody[j].w / (distance*distance);
                        vx[i] = vx[i] + T * F * (deltaX/distance) ;
                        vy[i] = vy[i] + T * F * (deltaY/distance) ;
                    }
                }
            }

            for(i=startPoint;i<job + startPoint;i++)
            {
                Nbody[i].x = Nbody[i].x + vx[i] * T;
                Nbody[i].y = Nbody[i].y + vy[i] * T;
                Nbody[i].vx = vx[i];
                Nbody[i].vy = vy[i];
            }

            for(i=0;i<job;i++)
            {
                local_Nbody[i].x = Nbody[startPoint+i].x;
                local_Nbody[i].y = Nbody[startPoint+i].y;
                local_Nbody[i].vy = Nbody[startPoint+i].vy;
                local_Nbody[i].vx = Nbody[startPoint+i].vx;
                local_Nbody[i].w = Nbody[startPoint+i].w;
            }

            MPI_Gather(local_Nbody, job, MPIBody, Nbody, job, MPIBody, 0, MPI_COMM_WORLD);

            XSetForeground(display, gc, WhitePixel(display,scr));
            for(i=0;i<body_num;i++) {
                if(Nbody[i].y <= HEIGHT && Nbody[i].x <= WIDTH)
                    XDrawPoint(display, pm, gc, Nbody[i].y, Nbody[i].x);
            }
            XCopyArea(display,pm,win,gc,0,0,WIDTH,HEIGHT,0,0);

            for (j = 1; j < size; j++)
                MPI_Send(Nbody, body_num, MPIBody, j, j, MPI_COMM_WORLD);

        }
        XFreePixmap(display,pm);
        XCloseDisplay(display);
    }
    else
    {
        MPI_Recv(Nbody, body_num, MPIBody, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(k=0;k<iteration_num;k++)
        {
            int startPoint = job * rank;
            for(i=startPoint;i<job + startPoint;i++)
            {
                for(j=0;j<body_num;j++)
                {
                    if (j==i) continue;
                    deltaX = Nbody[j].x - Nbody[i].x;
                    deltaY = Nbody[j].y - Nbody[i].y;
                    distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
                    if(distance == 0) continue;
                    if(distance >= 15) {
                        F = G * Nbody[j].w / (distance*distance);
                        vx[i] = vx[i] + T * F * (deltaX/distance) ;
                        vy[i] = vy[i] + T * F * (deltaY/distance) ;
                    }
                }
            }

            for(i=startPoint;i<job + startPoint;i++)
            {
                Nbody[i].x = Nbody[i].x + vx[i] * T;
                Nbody[i].y = Nbody[i].y + vy[i] * T;
                Nbody[i].vx = vx[i];
                Nbody[i].vy = vy[i];
            }

            for(i=0;i<job;i++)
            {
                local_Nbody[i].x = Nbody[startPoint+i].x;
                local_Nbody[i].y = Nbody[startPoint+i].y;
                local_Nbody[i].vy = Nbody[startPoint+i].vy;
                local_Nbody[i].vx = Nbody[startPoint+i].vx;
                local_Nbody[i].w = Nbody[startPoint+i].w;
            }

            MPI_Gather(local_Nbody, job, MPIBody, Nbody, job, MPIBody, 0, MPI_COMM_WORLD);
            MPI_Recv(Nbody, body_num, MPIBody, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    if(rank == 0)
    {
        endTime = MPI_Wtime();
        double total_time = endTime - startTime;
        printf("Execution Time is: %f seconds\n", total_time);
        printf("Finish Drawing...\n");
    }
    MPI_Finalize();
    return 0;
}
