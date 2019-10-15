#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

const int RMAX = 1000;
void GenerateList(int local_array[], int local_num);
void PrintList(int* Arr, int n);
int OddEvenSort(int local_array[], const int local_num, const int my_rank, int p, MPI_Comm comm);

int main(int argc, char* argv[]) {
	MPI_Comm comm;
	int rank;
	int processor_num;
	int global_num = 20;
	int local_num; 
	int* Arr = 0;
	int n = 0;
	clock_t start_time;
	clock_t end_time;
	double time;

	MPI_Init(&argc, &argv);
	comm = MPI_COMM_WORLD;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processor_num);

	int* global_array = (int*)malloc(sizeof(int) * global_num);
	local_num = global_num / processor_num;

	if (rank == 0) 
	{
		GenerateList(global_array, global_num);
		cout << "Name: Tian Min" << endl << "Student ID: 116010168" << endl << "Homework 1, Odd Even Sort, MPI Implementation" << endl;
		cout << "---------------Before Sort---------------" << endl;
		PrintList(global_array, global_num);
	}
	
	int* local_array = (int*)malloc(sizeof(int) *local_num);
	start_time = clock();
	MPI_Scatter(global_array, local_num, MPI_INT, local_array, local_num, MPI_INT, 0, comm);
	OddEvenSort(local_array,local_num, rank, processor_num, comm);
	MPI_Gather(local_array, local_num, MPI_INT, global_array, local_num, MPI_INT, 0, comm);
	end_time = clock();
	time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

	if (rank == 0)
	{

		cout << "---------------After Sort---------------" << endl;
		PrintList(global_array, global_num);
		cout << "---------------Time Analysis-------------" << endl;
		cout <<"The execution time is: " << setprecision(3) << time << " seconds" << endl;
		time = 0;
	}
	MPI_Finalize();
	return 0;
}



void GenerateList(int global_array[], int global_num) {
	int i;
	srand(clock());
	for (i = 0; i < global_num; i++) global_array[i] = rand() % RMAX;
}

void PrintList(int* Arr, int n){
	cout << "The list is: ";
	for (int count = 0; count < n; count++) {
		cout << Arr[count] << " | ";
	}
	cout << endl;
}


int OddEvenSort(int local_array[],const int local_num,const int my_rank,int p, MPI_Comm comm)
{
	int n = local_num;
	int temp = 0;
	int send_temp = 0;
	int recv_temp = 10001;
	int rightrank = (my_rank + 1) % p;
	int leftrank = (my_rank + p - 1) % p;

	for (int k = 0; k < p * n; k++)
	{
		if (k % 2 == 0)
		{
			for (int j = n - 1; j > 0; j -= 2)
			{
				if (local_array[j] < local_array[j - 1])
				{
					swap(local_array[j], local_array[j - 1]);
				}				
			}
		}
		else
		{
			for (int j = n - 2; j > 0; j -= 2)
			{
				if (local_array[j] < local_array[j - 1])
				{
					swap(local_array[j], local_array[j - 1]);
				}
			}
			if (my_rank != 0)
			{				
				send_temp = local_array[0];
				MPI_Send(&send_temp, 1, MPI_INT, leftrank, 0, comm);
				MPI_Recv(&recv_temp, 1, MPI_INT, leftrank, 0, comm, MPI_STATUS_IGNORE);
				if (recv_temp > local_array[0]) local_array[0] = recv_temp;
			}
			if (my_rank != p - 1) {
				int send_buff = local_array[local_num - 1];
				MPI_Recv(&recv_temp, 1, MPI_INT, rightrank, 0, comm, MPI_STATUS_IGNORE);  
				MPI_Send(&send_buff, 1, MPI_INT, rightrank, 0, comm);
				if (recv_temp < local_array[local_num - 1]) local_array[local_num - 1] = recv_temp;			
			}
		}
	}
	return 0;
}