#include "const.h"
#include "models.h"
#include "display.h"

#define varify(x, n) ( (x)>=0 && (x)<(n) )

#define start_time clock_gettime(CLOCK_MONOTONIC, &start);
#define end_time clock_gettime(CLOCK_MONOTONIC, &finish); 
#define time_elapsed_ns (long long)(finish.tv_sec-start.tv_sec)*1000000000 + finish.tv_nsec - start.tv_nsec
#define time_elapsed_s (double)(finish.tv_sec-start.tv_sec) + (double)(finish.tv_nsec - start.tv_nsec)/1000000000

int iteration,iter_cnt,x,y;
int enable = 0;
TemperatureField *field;
TemperatureField *tempField, *swapField;

int dx[4] = {0, -1, 0, 1};
int dy[4] = {1, 0, -1, 0};

void temp_iter(TemperatureField *field, int x)
{
	++iter_cnt;
	int i, j, d;
	for (i=0; i<field->x; ++i)
		for (j=0; j<field->y; ++j)
		{
			tempField->t[i][j] = 0;
			for (d=0; d<4; ++d)
				if ( varify(i+dx[d], field->x) && varify(j+dy[d], field->y) )
					tempField->t[i][j] += field->t[i+dx[d]][j+dy[d]];
				else
					tempField->t[i][j] += ROOM_TEMP;
			tempField->t[i][j] /= 4;
		}
	for (i=0;i<7*x/10;i++)
	{
		if(3*x/10 <i)
			tempField->t[0][i] = 100.0f;
	}
}

int main(int argc, char **argv)
{
	printf("Name: Tian Min\nStudent ID: 116010168\nAssignment 4, Heat Simulation, Sequential Implementation\n");
    if (argc<4)
    {
	    printf("Usage: %s x y iteration 1/0\n", argv[0]);
    }
    sscanf(argv[1], "%d", &x);
    sscanf(argv[2], "%d", &y);
    sscanf(argv[3], "%d", &iteration);
	sscanf(argv[4], "%d", &enable);

    field = malloc(sizeof(TemperatureField));
    tempField = malloc(sizeof(TemperatureField));
    newField(field, x, y,0,0);
    newField(tempField, x, y,0,0);
    initField(field);
	
	if(enable){
		printf("Graph is enabled...\n");
		XWindow_Init(field);
	}else{
		printf("Graph is disabled...\n");
	}
    

    struct timespec start, finish;
    start_time

    int iter;
    for (iter=0; iter<iteration; iter++)
    {
	temp_iter(field, x);
	swapField = field;
	field = tempField;
	tempField = swapField;
	
	if(enable){
		XRedraw(field);
	}
	
    }

    printf("Finished %d iterations.\n", iter_cnt);
    end_time;
    printf("The execution time is: %lf\n", time_elapsed_s);
    if(enable){
		printf("Displaying for 10s...\n");
		sleep(10);
	}
    return 0;
}
