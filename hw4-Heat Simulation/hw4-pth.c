#include "const.h"
#include "models.h"
#include "display.h"
#include <pthread.h>
#include <stdio.h>

#define varify(x, n) ( (x)>=0 && (x)<(n) )
#define start_time clock_gettime(CLOCK_MONOTONIC, &start);
#define end_time clock_gettime(CLOCK_MONOTONIC, &finish); 
#define time_elapsed_ns (long long)(finish.tv_sec-start.tv_sec)*1000000000 + finish.tv_nsec - start.tv_nsec
#define time_elapsed_s (double)(finish.tv_sec-start.tv_sec) + (double)(finish.tv_nsec - start.tv_nsec)/1000000000
#define NOT_FIRE_PLACE i

int iteration, threads;
int enable = 0;
TemperatureField *field;
TemperatureField *tempField, *swapField;
pthread_t *threadPool;
pthread_mutex_t *subThreadWakeUp, *subThreadFinished;
int *threadID, terminate;
double *error;
double  EPSILON;


int dx[4] = {0, -1, 0, 1};
int dy[4] = {1, 0, -1, 0};

int x, y, iter_cnt;

int min(int x, int y){ if (x<y) return x; return y; }

void* iterateLine(void* data)
{   
    int threadID = *((int*)data);
    while (1)
    {
	    pthread_mutex_lock(&subThreadWakeUp[threadID]);
	    if (terminate) break;
	    int blockSize = field->x/threads + !!(field->x%threads);
	    int lineStart = blockSize * threadID;
	    int lineEnd = min(blockSize*(threadID+1), field->x);
	    error[threadID]=0;

	    int i, j, d;
	    for (i=lineStart; i<lineEnd; ++i) 
		for (j=0; j<field->y; ++j)
		{
			tempField->t[i][j] = 0;
			for (d=0; d<4; ++d)
				if ( varify(i+dx[d], field->x) && varify(j+dy[d], field->y) )
					tempField->t[i][j] += field->t[i+dx[d]][j+dy[d]];
				else
					tempField->t[i][j] += ROOM_TEMP;
			tempField->t[i][j] /= 4;
			if (NOT_FIRE_PLACE)
				error[threadID] += fabs(tempField->t[i][j] - field->t[i][j]);
		}
	    pthread_mutex_unlock(&subThreadFinished[threadID]);
    }
    pthread_exit(NULL);
}

double temp_iter()
{
	++iter_cnt; 
	refreshField(field, 0, 0, field->x, field->y, field->x, field->y);
	int i;

	for (i=0; i<threads; ++i)
		pthread_mutex_unlock(&subThreadWakeUp[i]);
	for (i=0; i<threads; ++i)
		pthread_mutex_lock(&subThreadFinished[i]);

	double sumError = 0;
	for (i=0; i<threads; ++i)
		sumError += error[i];

	return sumError;
}

int main(int argc, char **argv)
{
	printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 4, Heat Simulation, Pthread Implementation\n");
    struct timespec start, finish;
    start_time

    if (argc<5)
    {
	    printf("Usage: %s x y iteration threads 1/0\n", argv[0]);
    }
    sscanf(argv[1], "%d", &x);
    sscanf(argv[2], "%d", &y);
    sscanf(argv[3], "%d", &iteration);
    sscanf(argv[4], "%d", &threads);
	sscanf(argv[5], "%d", &enable);
	EPSILON = 0.1;

    field = malloc(sizeof(TemperatureField));
    tempField = malloc(sizeof(TemperatureField));
    threadPool = malloc(sizeof(pthread_t)*threads);
    subThreadWakeUp = malloc(sizeof(pthread_mutex_t)*threads);
    subThreadFinished = malloc(sizeof(pthread_mutex_t)*threads);
    threadID = malloc(sizeof(int)*threads);
    error = malloc(sizeof(double)*threads);
    terminate = 0;
    field->x = y;
    field->y = x;

    int i;
    for (i=0; i<threads; ++i)
    {
	pthread_mutex_init(&subThreadWakeUp[i], NULL);
	pthread_mutex_init(&subThreadFinished[i], NULL);
	pthread_mutex_lock(&subThreadWakeUp[i]);
	pthread_mutex_lock(&subThreadFinished[i]);
	threadID[i] = i;
	pthread_create(&threadPool[i], NULL, iterateLine, &threadID[i]);
    }

    int iter;
    newField(field, x, x, 0, 0);
    newField(tempField, x, x, 0, 0);
    initField(field);
	if(enable){
		printf("Graph is enabled...\n");
		XWindow_Init(field);
	}else{
		printf("Graph is disabled...\n");
	}
    

	for (iter=0; iter<iteration; iter++)
    {	
	   double error = temp_iter();
	   if (error<EPSILON)
	   {
		printf("Finished. iteration=%d, error=%lf\n", iter, error);
		break;
	   }
	   swapField = field;
	   field = tempField;
	   tempField = swapField;
	   if(enable){
		   if(iter % 100 == 0) XRedraw(field);
	   }
	   
	}
    
    deleteField(field);
    deleteField(tempField);
    free(threadPool);
    for (i=0; i<threads; ++i)
    {
	    terminate = 1;
	    pthread_mutex_unlock(&subThreadWakeUp[i]);
    }
    
    printf("Finished %d iterations.\n", iter_cnt);
    end_time;
    printf("The execution time is: %lf\n", time_elapsed_s);
    pthread_exit(NULL);
	if(enable){
		printf("Displaying for 10s...\n");
		sleep(10);
	}
    return 0;
}
