#include "models.h"
#include "display.h"
#include <mpi.h>

#define varify(x, n) ( (x)>=0 && (x)<(n) )
#define start_time clock_gettime(CLOCK_MONOTONIC, &start);
#define end_time clock_gettime(CLOCK_MONOTONIC, &finish); 
#define time_elapsed_ns (long long)(finish.tv_sec-start.tv_sec)*1000000000 + finish.tv_nsec - start.tv_nsec
#define time_elapsed_s (double)(finish.tv_sec-start.tv_sec) + (double)(finish.tv_nsec - start.tv_nsec)/1000000000

int job;
int enable = 0;
int dx[4] = {0, -1, 0, 1};
int dy[4] = {1, 0, -1, 0};

int iteration,x,y,iter_cnt;
TemperatureField *field;
TemperatureField *tempField, *swapField;

void temp_iter(TemperatureField *field, int x)
{
	++iter_cnt;
	int i, j, d;
	for (i=0; i<field->x; ++i)
		for (j=0; j<field->y; ++j)
		{
			int cnt = 0;
			tempField->t[i][j] = 0;
			for (d=0; d<4; ++d)
				if ( varify(i+dx[d], field->x) && varify(j+dy[d], field->y) )
				{
					tempField->t[i][j] += field->t[i+dx[d]][j+dy[d]];
					++cnt;
				}
				// else
				// 	tempField->t[i][j] += ROOM_TEMP;
			tempField->t[i][j] /= cnt;
		}
	for (i=0;i<7*x/10;i++)
	{
		if(3*x/10 <i)
			tempField->t[0][i] = 100.0f;
	}
}

int main(int argc, char **argv){
    XInitThreads();

    struct timespec start, finish;
    start_time

	int i;
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc<4) {
	    printf("Usage: %s x y iteration 1/0\n", argv[0]);
    }
    sscanf(argv[1], "%d", &x);
    sscanf(argv[2], "%d", &y);
    sscanf(argv[3], "%d", &iteration);
	sscanf(argv[4], "%d", &enable);

    field = malloc(sizeof(TemperatureField));
    tempField = malloc(sizeof(TemperatureField));
    newField(field, x, x, 0, 0);
    newField(tempField, x, x, 0, 0);
    initField(field);

	if (rank == 0) {
		printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 4, Heat Simulation, MPI Implementation\n");
		if(enable){
			printf("Graph is enabled...\n");
			XWindow_Init(field);
		}else{
			printf("Graph is disabled...\n");
		}
        
	}
	
    job = x / size;
	if (x % size != 0) job++;

    int startx = rank * job;

    int iter;
	for (iter = 0; iter < iteration; iter++) {
		temp_iter(field, startx);
        MPI_Allgather(&(tempField->t[startx][0]), job*field->y, MPI_FLOAT, &(field->t[0][0]), job*field->y, MPI_FLOAT, MPI_COMM_WORLD);

		if (rank == 0) {
            for(i = x * 0.3; i < x * 0.7; i++)
			    field->t[0][i] = FIRE_TEMP;
			if(enable){
				if(iter % 100 == 0) XRedraw(field);
			}
		}
	} 

    if (rank == 0) {
		printf("Finished %d iterations.\n", iter_cnt);
		end_time;
		printf("The execution time is: %lf\n", time_elapsed_s);
		if(enable){
			printf("Displaying for 10s...\n");
			sleep(10);
		}
	}
    MPI_Finalize();
    return 0;
}
