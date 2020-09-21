/** 
* File:   GoodmanSJFL.c
* Simulate shortest-job first processor scheduling algorithms.
*
* Completion runningTime:  6 hours
* 
* @author Goodman
* @version 2020.09.17
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES
typedef struct Process {
    int processID;
    int tau;
    float alpha;
    int* t;
} Process;

////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
int numProcesses, numTicks, turnAroundTime = 0, waitingTime = 0, error = 0, runningTime = 0;
Process* processes = NULL;

////////////////////////////////////////////////////////////////////////////////
//FORWARD DECLARATIONS
void readFile(char* filename);
Process* readProcesses(FILE* file);
void printSJF();
void printSJFL();
void SJFSort(int* p, int* t, int n);
void SJFLSort(int* p, int* t, int* tau, int n);
void swap(int* x, int* y);
void terminate();

/////////////////////////////////////////////////////////////////////////////////

/**
* Loads data from process data file
* @param filename is the name of the file
*/
void readFile(char* filename){
    FILE* file = fopen(filename, "r");
    fscanf(file, "%d", &numTicks);
    fscanf(file, "%d", &numProcesses);
    processes = readProcesses(file);
    fclose(file);
}

/**
* Loads process data from data file
* @param file is the file pointer
*/
Process* readProcesses(FILE* file){
    int i, j;
    processes = (Process*)malloc(numProcesses * sizeof(Process));
    for(i = 0; i < numProcesses; i++){
        processes[i].t = (int*)malloc(sizeof(int) * numTicks);
        fscanf(file, "%d %d %f", &processes[i].processID, &processes[i].tau, &processes[i].alpha);
        for(j = 0; j < numTicks; j++) {
            fscanf(file, "%d", &processes[i].t[j]);
        }
    }
    return processes;
}

/**
* Prints SJF data to console
*/
void printSJF(){
    int i, j;
    int p[numProcesses], t[numProcesses];
    printf("==Shortest-Job-First==\n");
    for(i = 0; i < numTicks; i++) {
        printf("Simulating %dth tick of processes @ time %d:\n", i, runningTime);
        for (j = 0; j < numProcesses; j++) {
            p[j] = processes[j].processID;
            t[j] = processes[j].t[i];
        }
        SJFSort(p, t, numProcesses);
        for (j = 0; j < numProcesses; j++){
            runningTime += processes[j].t[i];
            printf("  Process %d took %d.\n", p[j], t[j]);
        }
        waitingTime += t[0];
        turnAroundTime = runningTime + waitingTime;
    }
    printf("Turnaround time: %d\n", turnAroundTime);
    printf("Waiting time: %d\n", waitingTime);
    runningTime = 0;
    turnAroundTime = 0;
    waitingTime = 0;
}

/**
* Prints SJFL data to console
*/
void printSJFL(){
    int i, j, tau[numProcesses];
    float diff;
    int p[numProcesses], t[numProcesses];
    printf("==Shortest-Job-First Live==\n");
    for(i = 0; i < numTicks; i++) {
        printf("Simulating %dth tick of processes @ time %d:\n", i, runningTime);
        for (j = 0; j < numProcesses; j++) {
            p[j] = processes[j].processID;
            t[j] = processes[j].t[i];
            tau[j] = processes[j].tau;
        }
        SJFLSort(p, t, tau, numProcesses);
        for (j = 0; j < numProcesses; j++){
            runningTime += processes[j].t[i];
            printf("  Process %d was estimated for %d and took %d.\n", p[j], processes[p[j]].tau, t[j]);
            diff = (float)processes[p[j]].tau - (float)t[j];
            error += abs((int)diff);
            diff = diff * processes[p[j]].alpha;
            if(diff < 0)
                processes[p[j]].tau = processes[p[j]].tau - (int)diff;
            else
                processes[p[j]].tau = processes[p[j]].tau - abs((int)round((double)diff));
        }
        waitingTime += t[0];
        turnAroundTime = runningTime + waitingTime;
    }
    printf("Turnaround time: %d\n", turnAroundTime);
    printf("Waiting time: %d\n", waitingTime);
    printf("Estimation Error: %d\n", error);
    runningTime = 0;
    turnAroundTime = 0;
    waitingTime = 0;
    error = 0;
}

/**
* Sorts processes and their respective attributes in SJF algorithm
* @param p is a pointer to an array of process IDs
* @param t is a pointer to an array of process t values
* @param n is the size of the p and t arrays
*/
void SJFSort(int *p, int *t, int n){
    int i, j, min;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++)
            if (t[j] < t[min])
                min = j;
        swap(&t[min], &t[i]);
        swap(&p[min], &p[i]);
    }
}

/**
* Sorts processes and their respective attributes in SJF algorithm
* @param p is a pointer to an array of process IDs
* @param t is a pointer to an array of process t values
* @param tau is a pointer to an array of process tau values
* @param n is the size of the p, t and tau arrays
*/
void SJFLSort(int* p, int* t, int* tau, int n){
    int i, j, min;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++)
            if (tau[j] < tau[min])
                min = j;
        swap(&tau[min], &tau[i]);
        swap(&t[min], &t[i]);
        swap(&p[min], &p[i]);
    }
}

/**
* Helper function for sorting processes
* @param x is a pointer to a value to be swapped with y
* @param y is a pointer to a value to be swapped with x
*/
void swap(int* x, int* y){
    int temp = *x;
    *x = *y;
    *y = temp;
}

/**
* Frees memory and exits program
*/
void terminate(){
    int i;
    for(i = 0; i < numProcesses; i++)
        free(processes[i].t);
    free(processes);
    processes = NULL;
    exit(1);
}

/**
* main
*/
int main(int argc, char* argv[]){
    char* datafile;
    int i, dflen;
    datafile = argv[1];
    if(datafile != NULL){
        dflen = strlen(datafile);
        if(dflen >= 5
           && (strcmp(&datafile[dflen-4], ".txt") == 0)
           && (access(datafile, F_OK) != -1)){
            printf("Importing data from %s\n\n", datafile);
            readFile(datafile);
        } else {
            printf("Data file has an invalid name or does not exist.\n");
            exit(1);
        }
    } else {
        printf("No data file name provided. This is a required field.\n");
        exit(1);
    }
    printSJF();
    printf("\n");
    printSJFL();
    terminate();
    return 0;
}