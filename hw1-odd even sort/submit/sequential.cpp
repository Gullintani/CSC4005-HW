#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

const int RMAX = 1000;

void oddEvenSord(int arr[], int n){
    bool isSorted = false;
    
    while(!isSorted){
        isSorted = true;
        
        for(int i = 1; i <= n -2; i = i + 2){
            if(arr[i] > arr[i + 1]){
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
        
        for(int i = 0; i < n - 2; i = i + 2){
            if(arr[i] > arr[i + 1]){
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
    return;
}

void printArray(int arr[], int n){
    for(int i = 0; i < n; i++){
        cout << arr[i] << " ";
    }
   cout << endl;
}

void Generate_list(int Gobal_A[], int gobal_n) {
	srand(clock());
	for (int i = 0; i < gobal_n; i++) Gobal_A[i] = rand() % RMAX;
}

void printInform(){
    cout << "Name: Tian Min" << endl  << "Student ID: 116010168" << endl << "Homework 1, Odd Even Sort, Sequential Implementation" << endl;
}

int main( )
{
   clock_t start, end;
   int n = 20;
   int num_list[n];
   Generate_list(num_list, n);
   
   printInform();
   cout << "---------------Before Sort---------------" << endl;
   printArray(num_list, n);
   
   start = clock();
   oddEvenSord(num_list, n);
   end = clock();
   
   cout << "---------------Result---------------" << endl;
   printArray(num_list, n);
   cout << "Runtime: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds" << endl;
   
   return(0);
}