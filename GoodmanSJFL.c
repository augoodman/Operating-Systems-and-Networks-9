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
// MACROS: CONSTANTS
#define MAX_STUDENT_NAME_LENGTH 20
#define MAX_COURSE_NAME_LENGTH 10
#define MAX_TEACHER_NAME_LENGTH 20
#define MAX_COMMENT_LENGTH 20
#define MAX_ASSIGN_NAME_LENGTH 20
#define MAX_FILENAME_LENGTH 30

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES
typedef enum AcademicLevel {Freshman = 0, Sophomore, Junior, Senior} AcademicLevel;
typedef struct ScoreStruct {
    int assignID;
    int studentID;
    float score;
    char comment[100];
} ScoreStruct;
typedef struct Assign {
    int assignID;
    char assignName[100];
} Assign;
typedef struct Student {
    int studentID;
    char studentName[100];
    AcademicLevel level;
} Student;
typedef struct Course {
    int courseID;
    char courseName[100];
    char instructorName[100];
    int numAssign;
    Assign* assignments;
    ScoreStruct* scores;
} Course;

////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
//place to store student information
Student* students = NULL;
//place to store course information
Course* courses = NULL;
int numCourses, numStudents;

////////////////////////////////////////////////////////////////////////////////
//FORWARD DECLARATIONS

// the following should be used to read student/course data in from the file 
void readFile(char* filename);
void readString(FILE* file, char** dst, int max_length);
Student* readStudents(FILE* file);
Course* readCourses(FILE* file);
Assign* readAssigns(FILE* file, int numAssigns);
ScoreStruct** readScores(FILE* file, int numAssigns);

// the following should be used to free all heap data allocated during the programs runtime
// and handle dangling pointers
void terminate();
void studentsDestructor();
void coursesDestructor();
void assignsDestructor(Assign** assigns, int numAssign);
void scoresDestructor(ScoreStruct** scores, int numAssigns);

// the following should be used to cleanly print the data used in the program 
void printStudents();
void printAssigns(Assign* assigns, int numAssigns);
void printGrades(ScoreStruct* scores, int numAssigns);
void printCourse(Course course);

// the following are mostly complete functions that define and 
// control the CLI menu associated with the program
void mainMenu();
void mainMenuBranch(int option);
void subMenu(Course course);
void subMenuBranch(int option, Course course);

// these are the 'special' functions that you are being asked to implement
void studentMenu(Course course);
void getStudentScores(Course course, int studentNo);
void assignmentMenu(Course course);
void getAssignmentScore(Course course, int assignmentNo);

// this is an optional function that parses and executes instructions defined in an input file
void performInstructions(char* iFile);

// this is a utility function to be called when input filenames are invalid
void printUsage();

/////////////////////////////////////////////////////////////////////////////////
//IMPLEMENTATION

/**
* Loads data from student/course data file
* @param filename is the name of the file
*/
void readFile(char* filename){
    FILE* file = fopen(filename, "r");
    //FOR YOU TO IMPLEMENT
    students = readStudents(file);
    courses = readCourses(file);
    fclose(file);
}

/**
* Loads data from student portion of data file
* @param file is the file pointer
*/
Student* readStudents(FILE* file){
    int i;
    fscanf(file, "%d", &numStudents);
    students = (Student*)malloc(numStudents * sizeof(Student));
    for(i = 0; i < numStudents; i++)
        fscanf(file, "%d %s %d", &students[i].studentID, students[i].studentName,
               (int*)&students[i].level);
    return students;
}

/**
* Loads data from course portion of data file
* @param file is the file pointer
*/
Course* readCourses(FILE* file){
    int i;
    fscanf(file, "%d", &numCourses);
    courses = (Course*)malloc(numCourses * sizeof(Course));
    for(i = 0; i < numCourses; i ++){
        fscanf(file, "%d %s %s %d", &courses[i].courseID, courses[i].courseName,
               courses[i].instructorName, &courses[i].numAssign);
        readAssigns(file, courses[i].numAssign);
    }
    return courses;
}

/**
* Loads data from assignment portion of course data in data file
* @param file is the file pointer
* @param numAssigns is the number of assignments in a course
*/
Assign* readAssigns(FILE* file, int numAssigns){
    int i, j;
    Assign temp;
    courses->assignments = (Assign*)malloc(courses->numAssign * sizeof(Assign));
    for(i = 0; i < courses->numAssign; i++)
        fscanf(file, "%d %s", &courses->assignments[i].assignID,
               courses->assignments[i].assignName);
    for (i = 0; i < courses->numAssign; i++)
        for (j = i + 1; j < courses->numAssign; j++)
            if (courses->assignments[i].assignID > courses->assignments[j].assignID) {
                temp = courses->assignments[i];
                courses->assignments[i] = courses->assignments[j];
                courses->assignments[j] = temp;
            }
    readScores(file, courses->numAssign);
    return courses->assignments;
}

/**
* Loads data from scores portion of course data in data file
* @param file is the file pointer
* @param numAssigns is the number of assignments in a course
*/
ScoreStruct** readScores(FILE* file, int numAssigns){
    int i, j;
    ScoreStruct temp;
    courses->scores = (ScoreStruct *) malloc(numStudents * courses->numAssign *
                                             sizeof(ScoreStruct));
    for (i = 0; i < numStudents * courses->numAssign; i++)
        fscanf(file, "%d %d %f %s", &courses->scores[i].assignID,
               &courses->scores[i].studentID, &courses->scores[i].score,
               courses->scores[i].comment);
    for (i = 0; i < numStudents * courses->numAssign; i++)
        for (j = i + 1; j < numStudents * courses->numAssign; j++)
            if (courses->scores[i].assignID > courses->scores[j].assignID) {
                temp = courses->scores[i];
                courses->scores[i] = courses->scores[j];
                courses->scores[j] = temp;
            }
    for (i = 0; i < numStudents * courses->numAssign; i++)
        for (j = i + 1; j < numStudents * courses->numAssign; j++)
            if (courses->scores[i].studentID > courses->scores[j].studentID) {
                temp = courses->scores[i];
                courses->scores[i] = courses->scores[j];
                courses->scores[j] = temp;
            }
    return &courses->scores;
}

/**
* Calls destructor functions to free memory and exits program
*/
void terminate(){
    //FREE EVERYTHING HERE
    int i;
    for(i = 0; i < numCourses; i++) {
        assignsDestructor(&courses[i].assignments, courses[i].numAssign);
        scoresDestructor(&courses[i].scores, courses[i].numAssign);
    }
    studentsDestructor();
    coursesDestructor();
    exit(1);
}

/**
* Frees student struct memory
*/
void studentsDestructor(){
    free(students);
}

/**
* Frees course struct memory
*/
void coursesDestructor(){
    free(courses);
}

/**
* Frees assignment struct memory
*/
void assignsDestructor(Assign** assigns, int numAssigns){
    free(*assigns);
}

/**
* Frees score struct memory
*/
void scoresDestructor(ScoreStruct** scores, int numAssigns){
    free(*scores);
}

/**
* Converts level enum to appropriate strings and prints student data to console
*/
void printStudents(){
    int i;
    char* level;
    char* freshman = "(Freshman)";
    char* sophomore = "(Sophomore)";
    char* junior = "(Junior)";
    char* senior = "(Senior)";
    for(i = 0; i < numStudents; i++){
        if(students[i].level == 1)
            level = freshman;
        else if(students[i].level == 2)
            level = sophomore;
        else if(students[i].level == 3)
            level = junior;
        else level = senior;
        printf("  %d %s %s\n", students[i].studentID, students[i].studentName,
               level);
    }
}

/**
* Prints assignment data to console
* @param assigns is the array of assignment structs to be printed
* @param numAssigns is the number of assignments in a course
*/
void printAssigns(Assign* assigns, int numAssigns){
    int i;
    printf("Assigns:\n");
    for(i = 0; i < numAssigns; i++)
        printf("  %d %s\n", assigns[i].assignID, assigns[i].assignName);
}

/**
* Prints grade data to console
* @param scores is the array of score structs to be printed
* @param numAssigns is the number of assignments in a course
*/
void printGrades(ScoreStruct* scores, int numAssigns){
    int i;
    printf("Grade Data:\n");
    for(i = 0; i < numStudents * numAssigns; i++)
        printf("  %d %d %.2f %s\n", scores[i].studentID, scores[i].assignID,
               scores[i].score, scores[i].comment);
}

/**
* Prints course data to console
* @param course is the course struct to be printed
*/
void printCourse(Course course){
    printf("\nCourse ID: %d\n", course.courseID);
    printf("Course Name: %s\n", course.courseName);
    printf("Teacher: %s\n", course.instructorName);
    printAssigns(course.assignments, course.numAssign);
    printGrades(course.scores, course.numAssign);
}

/**
* Implements main menu functionality, which allows user to select a course to interact with
*/
_Noreturn void mainMenu(){
    int input_buffer;
    printf("Course Searcher");
    do {
        printf("\n-----------------------------------\n");
        printf("Course Options");
        printf("\n-----------------------------------\n");
        int i;
        for(i = 0; i < numCourses; i++)
            //FOR YOU TO IMPLEMENT
            printf("   %d %s\n", courses[i].courseID, courses[i].courseName);
        printf("   0 REPEAT OPTIONS\n");
        printf("  -1 TERMINATE PROGRAM\n");
        printf("Please enter your choice ---> ");
        scanf(" %d", &input_buffer);
        mainMenuBranch(input_buffer);
    } while (1);
}

/**
* Handles menu options of main menu
* @param option is the chosen operation's option #
*/
void mainMenuBranch(int option){
    if (option < -1 || option > numCourses){
        printf("Invalid input. Please try again...\n");;
        while(option < -1 || option > numCourses){
            printf("Please enter a valid option ---> ");
            scanf(" %d", &option);
        }
    }
    if(option == -1){
        printf("Terminating program...\n");
        terminate();
    } else if (option == 0)
        printf("Repeating options...\n");
    else {
        Course course = courses[option - 1];
        //FOR YOU TO IMPLEMENT
        printf("Course with name %s selected.\n", course.courseName);
        subMenu(course);
    }
}

/**
* Implements sub menu functionality, which allows users to select how they want to interact with course
* @course is the course struct to be queried
*/
void subMenu(Course course){
    int input_buffer;
    do {
        printf("\n-----------------------------------\n");
        printf("Menu Options");
        printf("\n-----------------------------------\n");
        printf("   1 Get a student's final grades in the course\n");
        printf("   2 Get the average grade of an assignment in the course\n");
        printf("   3 Print all course data\n");
        printf("   0 REPEAT OPTIONS\n");
        printf("  -1 RETURN TO PREVIOUS MENU\n");
        printf("  -2 TERMINATE PROGRAM\n");
        printf("Please enter your choice ---> ");
        scanf(" %d", &input_buffer);
        subMenuBranch(input_buffer, course);
    } while (input_buffer != -1);
}

/**
* Handles menu options of submenu
* @param option is the chosen operation's option #
* @param course is the course struct to be queried
*/
void subMenuBranch(int option, Course course){
    if (option < -2 || option > 3){
        printf("Invalid input. Please try again...\n");;
        while(option < -2 || option > 3){
            printf("Please enter a valid option ---> ");
            scanf(" %d", &option);
        }
    }
    if(option == -2){
        printf("Terminating program...\n");
        terminate();
    } else if (option == -1)
        printf("Returning to previous menu...\n");
    else if (option == 0)
        printf("Repeating options...\n");
    else if (option == 1)
        //FOR YOU TO IMPLEMENT
        studentMenu(course);
    else if (option == 2)
        //FOR YOU TO IMPLEMENT
        assignmentMenu(course);
    else if (option == 3)
        //FOR YOU TO IMPLEMENT
        printCourse(course);
}

/**
* Handles student menu options
* @param course is the course struct to be queried
*/
void studentMenu(Course course){
    int input_buffer;
    do {
        printf("\nPlease choose from the following students:\n");
        printStudents();
        printf("  0 RETURN TO PREVIOUS MENU\n");
        printf("Please enter your choice ---> ");
        scanf(" %d", &input_buffer);
        if(input_buffer == 0)
            subMenu(course);
        getStudentScores(course, input_buffer);
    } while (1);
}

/**
* Handles student menu options
* @param course is the course struct to be queried
* @param studentNo identifies the student struct to queried
*/
void getStudentScores(Course course, int studentNo){
    int i, j, index = 0;
    ScoreStruct* s;
    Assign* a;
    Assign temp;
    float total = 0;
    int numScores = 0;
    studentNo--;
    printf("\n%s's assignment specific grades were:\n", students[studentNo].studentName);
    printf("\n  Assign Name     Score           Comment\n");
    printf("-----------------------------------------------\n");
    a = malloc(course.numAssign * sizeof(Assign));
    s = malloc(course.numAssign * sizeof(ScoreStruct));
    for(i = 0; i < numStudents * course.numAssign; i++)
        if(course.scores[i].studentID == studentNo + 1)
            for(j = 0; j < course.numAssign; j++)
                if(course.assignments[j].assignID == course.scores[i].assignID){
                    s[index] = course.scores[i];
                    a[index] = course.assignments[j];
                    index++;
                }
    for(i = 0; i < course.numAssign; i++)
        for(j = i + 1; j < course.numAssign ; j++)
            if(strcmp(a[i].assignName, a[j].assignName) > 0){
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
    for(i = 0; i < course.numAssign; i++)
        for(j = 0; j < course.numAssign; j++){
            if(a[j].assignID == s[i].assignID && s[i].score == 100)
                printf("  %s           %.2f          %s\n", a[j].assignName,
                       s[i].score, s[i].comment);
            else if(a[j].assignID == s[i].assignID && s[i].score < 10)
                printf("  %s           %.2f            %s\n", a[j].assignName,
                       s[i].score, s[i].comment);
            else if(a[i].assignID == s[j].assignID)
                printf("  %s           %.2f           %s\n", a[i].assignName,
                       s[j].score, s[j].comment);
            total += s[i].score;
            numScores++;
        }
    float average = total / numScores;
    printf("\n%s's final grade was %.2f.\n", students[studentNo].studentName, average);
    free(a);
    free(s);
}

/**
* Handles assignment menu options
* @param course is the course struct to be queried
*/
void assignmentMenu(Course course){
    int input_buffer;
    do {
        printf("\nPlease choose from the following assignments:\n");
        int i;
        for(i = 0; i < course.numAssign; i++)
            printf("  %d %s\n", course.assignments[i].assignID,
                   course.assignments[i].assignName);
        printf("  0 RETURN TO PREVIOUS MENU\n");
        printf("Please enter your choice ---> ");
        scanf(" %d", &input_buffer);
        if(input_buffer == 0)
            subMenu(course);
        getAssignmentScore(course, input_buffer);
    } while (1);
}

/**
* Handles student menu options
* @param course is the course struct to be queried
* @param assignmentNo identifies the assignment struct to queried
*/
void getAssignmentScore(Course course, int assignmentNo){
    int i;
    char* name;
    float total = 0;
    int numScores = 0;
    for(i = 0; i < course.numAssign; i++)
        if(course.assignments[i].assignID == assignmentNo)
            name = course.assignments[i].assignName;
    for(i = 0; i < numStudents * course.numAssign; i++)
        if(course.scores[i].assignID == assignmentNo){
            total += course.scores[i].score;
            numScores++;
        }
    float average = total / numScores;
    printf("\nThe average grade on %s was %.2f.\n", name, average);
}

/**
* Performs instructions from instruction file to query data from a data file
* @param iFile is the name of the file
*/
void performInstructions(char* iFile){
    FILE* file = fopen(iFile, "r");
    int i, numInst;
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    int input[size];
    for(i = 0; i < size/2; i++)
        fscanf(file, "%d", &input[i]);
    numInst = input[0];
    for(i = 0; i < numInst; i++){
        printf("\nProcessing instruction with courseNum %d, studentNum %d, and assignmentNum %d",
               input[(i * 3) + 1], input[(i * 3) + 2], input[(i * 3) + 3]);
        if(input[(i * 3) + 2] == 0 && input[(i * 3) + 3] == 0)
            printCourse(courses[input[(i * 3) + 1] - 1]);
        else if(input[(i * 3) + 2] == 0 && input[(i * 3) + 3] != 0)
            getAssignmentScore(courses[input[(i * 3) + 1] - 1], input[(i * 3) + 3]);
        else if(input[(i * 3) + 2] != 0 && input[(i * 3) + 3] == 0)
            getStudentScores(courses[input[(i * 3) + 1] - 1], input[(i * 3) + 2]);
        else {
            float* s;
            char a[100];
            int j, k, l;
            for(j = 0; j < numStudents * courses[input[(i * 3) + 1] - 1].numAssign; j++)
                if(courses[input[(i * 3) + 1] - 1].scores[j].studentID == input[(i * 3) + 2])
                    for(k = 0; k < 2; k++)
                        if(courses[input[(i * 3) + 1] - 1].assignments[k].assignID == input[(i * 3) + 3])
                            for (l = 0; l < courses[input[(i * 3) + 1] - 1].numAssign; l++)
                                if (courses[input[(i * 3) + 1] - 1].scores[l].assignID == input[(i * 3) + 3]) {
                                    s = &courses[input[(i * 3) + 1] - 1].scores[l].score;
                                    strcpy(a, courses[input[(i * 3) + 1] - 1].assignments[k].assignName);
                                }
            printf("\nStudent with name %s received a %.2f on assignment %s\n",
                   students[input[(i * 3) + 2] - 1].studentName, *s, a);
        }
    }
    printf("\nEnd of instructions reached. Terminating.\n");
    fclose(file);
    terminate();
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
    char* instructionfile;
    int opt;
    while((opt = getopt(argc, argv, ":d:i:")) != -1)
        switch(opt){
            case 'd':
                datafile = optarg;
                break;
            case 'i':
                instructionfile = optarg;
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
            print_usage();
            exit(1);
        }
    } else {
        printf("No data file name provided. This is a required field.\n");
        print_usage();
        exit(1);
    }
    int iflen;
    int ifval;
    if(instructionfile != NULL){
        iflen = strlen(instructionfile);
        if(iflen >= 5
           && (strcmp(&instructionfile[iflen-4], ".txt") == 0)
           && (access(instructionfile, F_OK) != -1)){
            printf("Performing instructions defined in %s:\n", instructionfile);
            //uncomment below if doing this optional part of the assignment
            performInstructions(instructionfile);
        } else {
            printf("Instruction file has an invalid name or does not exist.\n");
            print_usage();
            exit(1);
        }
    } else {
        printf("No instruction file provided. Using CLI:\n");
        mainMenu();
    }
    return 0;
}
#endif