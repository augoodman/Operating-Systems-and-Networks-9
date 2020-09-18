/** 
* File:   GoodmanSJFL.c
* Simulate shortest-job first processor scheduling algorithms.
*
* Completion time:  ___ hours
* 
* @author Goodman
* @version 2020.09.17
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
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
    int* e;
} Process;

////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
Process* processes = NULL;
int numProcesses, numTicks, turnAroundTime, waitingTime, error, time = 0;

////////////////////////////////////////////////////////////////////////////////
//FORWARD DECLARATIONS

void readFile(char* filename);
Process* readProcesses(FILE* file);
int addTime(int t);
int estimateTime(int t);
int calcTurnaround();
int calcWaiting();
int calcError();
void terminate();
void printSJF(Process* p);
void printSJFL(Process* p); //and maybe some other data

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
* Calls destructor functions to free memory and exits program
*/
void terminate(){
    int i;
    for(i = 0; i < numProcesses; i++) {
        free(processes[i].t);
        free(processes[i].e);
    }
    free(processes);
    processes = NULL;
    exit(1);
}

/**
* Prints SJF data to console
* @param course is the course struct to be printed
*/
void printSJF(Process* p){
    int i, j;
    printf("==Shortest-Job-First==\n");
    for(i = 0; i < numTicks; i++){
        printf("Simulating %dth tick of processes @ time %d:\n", i, time);
        for(j = 0; j < numProcesses; j++)
            printf("\tProcess %d took %d.\n", j, p[j].t[i]);
    }
    printf("Turnaround time: %d\n", turnAroundTime);
    printf("Waiting time: %d\n", waitingTime);
}


/**
* Prints SJFL data to console
* @param course is the course struct to be printed
*/
void printSJFL(Process* p){
    int i, j;
    printf("==Shortest-Job-First Live==\n");
    for(i = 0; i < numTicks; i++){
        printf("Simulating %dth tick of processes @ time %d:\n", i, time);
        for(j = 0; j < numProcesses; j++)
            printf("\tProcess %d was estimated for %d and took %d.\n", j, p[j].e[i], p[j].t[i]);
    }
    printf("Turnaround time: %d\n", turnAroundTime);
    printf("Waiting time: %d\n", waitingTime);
}

/**
* Prints basic usage instructions for the program to the command line
*/
void print_usage(){
    printf("USAGE:\n./LastNameCourseReader -d <data_file_name(char*)> -c <instruction_file_name(char*)>\n");
    printf("-d refers to the required input data file containing student & course information; this must be a valid .txt file\n");
    printf("-i refers to the optionally supported 'instruction file' that provides directions for how the program should execute without CLI input; \n\t must be a valid .txt.file\n");
}

/**
* main
*/
int main(int argc, char* argv[]){
    char* datafile;
    int opt;
    while((opt = getopt(argc, argv, ":d:")) != -1)
        switch(opt){
            case 'd':
                datafile = optarg;
                break;
            case ':':
                printf("option needs a value\n");
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }
    for (; optind < argc; optind++)
        printf("Given extra arguments: %s\n", argv[optind]);
    int dflen;
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
    printSJF(processes);
    printf("\n");
    //printSJFL(processes);
    return 0;
}