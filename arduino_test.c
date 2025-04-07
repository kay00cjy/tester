// Objective: The following code serves as a testing framework for the computational
// aspects of the medication reminder system. It is designed to simulate the functionality 
// of the Arduino code in a C testing interface with a user-input based appraoch, allowing 
// for easier debugging without the need for hardware components. The code specifically 
// focuses on the functions "storeSchedule", "conversion", "checkTime", "calc", 
// and the input for "rotateServo". Each function implemented with the same logic as prior
// code directly in section, but without the format structure of functions.
// An additional timer command has been implemented to simulate the elasped time for the search function. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// define parameters
#define MAX_COMP    15          // 14 compartments for medication, 1 compartment for callibration
#define BUFFER      10000       // max length for incoming serial data
#define ANGLE       11          // rotation angle to servo motor for each compartment

// create global variables
ScheduleEntry schedule[MAX_COMP];  // Array to store schedule entries
int entries = 0;

// create a new datastype to store all entries in Medication Schedule
typedef struct {
    int compartment;
    char date[11];                  // format: YYYY-MM-DD
    char time[6];                   // format: HH:MM
    char name[50];                  // medication name
    char effect[1000];              // medication effect
    unsigned long reminderTime;     // time (in milliseconds) to take medication
} ScheduleEntry;


// create a function to convert data input to millis()
unsigned long conversion (char *date, char *time){
    int year, month, day, hours, minutes;

    // make sure all required fields are inputted by user
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) return 0;
    if (sscanf(time, "%d:%d", &hours, &minutes) != 2) return 0;

    unsigned long dateConvert = (unsigned long)3600000 * 24 * (day + 31 * (month + (12 * year)));
    unsigned long timeConvert = (unsigned long)3600000 * hours + (unsigned long)60000 * minutes;

    return dateConvert + timeConvert;
}

int calc(int i) {
    return ANGLE * schedule[i].compartment; //compartments evenly spaced;
}

void addEntry() {
    // variables to store input data
    int compartment;
    char buffer[BUFFER]; 
    char date[11], time[6], name[50], effect[1000];

    if (entries >= MAX_COMP) {
        printf("Schedule is full.\n");
        return;
    }

    // user input for schedule entry
    printf("Enter compartment number (1-14): ");
    fgets(buffer, sizeof(buffer), stdin);
    sscanf(buffer, "%d", &compartment);

    printf("Enter date (YYYY-MM-DD): ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0';
    getchar(); 

    printf("Enter time (HH:MM): ");
    fgets(time, sizeof(time), stdin);
    time[strcspn(time, "\n")] = '\0';
    getchar(); 

    printf("Enter medication name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    getchar(); 

    printf("Enter medication effect: ");
    fgets(effect, sizeof(effect), stdin);
    effect[strcspn(effect, "\n")] = '\0';

    // store the schedule entry
    ScheduleEntry entry;
    entry.compartment = compartment;
    strncpy(entry.date, date, 10);
    strncpy(entry.time, time, 5);
    strncpy(entry.name, name, 49);
    strncpy(entry.effect, effect, 999);

    // Convert date and time to milliseconds for trigger time
    entry.reminderTime = conversion(date, time);
    entries++;

    printf("Entry stored successfully.\n\n");
}

void search() {
    // timer to measure elapsed time
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);

    // user input to for compartment to search for
    int target;
    printf("Enter compartment number to search for: ");
    scanf("%d", &target);
    getchar();

    // start timer
    QueryPerformanceCounter(&start);

    for (int i = 0; i < entries; i++) {
        if (schedule[i].compartment == target) {

            int rotate = calc(i); 

            // stop timer once found
            QueryPerformanceCounter(&end);
            double elapsedTime = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;

            printf("\nFound matching compartment:\n");
            printf("Compartment: %d\n", schedule[i].compartment);
            printf("Medication: %s\n", schedule[i].name);
            printf("Effect: %s\n", schedule[i].effect);
            printf("Rotation angle: %d degrees\n", rotate);
            printf("Elapsed time to find: %.5f ms\n", elapsedTime);
            return;
        }
    }

    printf("No matching compartment found.\n");
}

int main() {

    int numEntries;
    printf("How many entries do you want to input? ");
    scanf("%d", &numEntries);
    getchar();

    for (int i = 0; i < numEntries; i++) {
        printf("\n--- Entry %d ---\n", i + 1);
        addEntry();
    }

    search();

    return 0;
}