#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define parameters
#define MAX_COMP    15          // 14 compartments for medication, 1 compartment for callibration
#define BUFFER      10000       // max length for incoming serial data
#define ANGLE       11          // rotation angle to servo motor for each compartment

// define pins (modular depending on hardware placement)
#define SOUND   9
#define SERVO   10

// create global variables
ScheduleEntry schedule[MAX_COMP];  // Array to store schedule entries
int entries = 0;
unsigned long startTime = 0;
int timeSet = 0;  // boolean variable (0 = false, 1 = true)

// create a new datastype to store all entries in Medication Schedule
typedef struct {
    int compartment;
    char date[11];                  // format: YYYY-MM-DD
    char time[6];                   // format: HH:MM
    char name[50];                  // medication name
    char effect[1000];               // medication effect
    unsigned long reminderTime;     // time (in milliseconds) to take medication
} ScheduleEntry;

/** create a function to read data from serial port
void readSerialData(char *input) {
    int index = 0;

    // Read until newline or buffer full
    while (Serial.available() && index < BUFFER - 1) {
        char c = Serial.read(); // reads data line by line (row - wise)
        if (c == '\n') break; // end of line
        input[index++] = c; // stores data  
    }
    input[index] = '\0';  // standard terminator so the program will not blow up
} */

// create a function for storing schedule entries
void storeSchedule(char *input) {
    // Variables to store input data
    int compartment;
    char date[11], time[6], name[50], effect[1000];

    // Input received data (Format: "compartment,date,time,medication" excluding terminator '\n')
    if (sscanf(input, "%d,%10[^,],%5[^,],%49[^\n],%999[^\n]", &compartment, date, time, name, effect) == 5) { 
        
        if (compartment == 0) {
            // Compartment 0 is for calibration
            calibrate(date, time);

        } else if (entries < MAX_COMP - 1) {
            // Store the schedule entry
            schedule[entries].compartment = compartment;
            strncpy(schedule[entries].date, date, 10);
            strncpy(schedule[entries].time, time, 5);
            strncpy(schedule[entries].name, name, 49);
            strncpy(schedule[entries].effect, effect, 999);

            // Convert date and time to milliseconds for trigger time
            schedule[entries].reminderTime = conversion(date, time);
            entries++;
        }
    }
}

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

// create a function for calibration
void calibrate(char *date, char *time) {
    startTime = conversion(date, time);
    timeSet = 1; // time successfully set!
}

// create function to compare current time to dosage times
unsigned long passingtime(){
    return millis() - startTime;
}

// create function to that goes through entries and checks if it's time to take medication
void checkTime() {
    for (int i = 0; i < entries; i++) {
        unsigned long timePast = passingtime();

        if (timePast >= schedule[i].reminderTime) {
            playSound();
            displayMedication(i);

            int rotation = calc(i);
            rotateServo(rotation);
        }
    }
}

// create function to play reminder sound
void playSound() {
    /** digitalWrite(SOUND, HIGH);
    delay(10000);  // play sound for 10 seconds
    digitalWrite(SOUND, LOW);
    delay(20000); // wait for 20 seconds
    digitalWrite(SOUND, HIGH);
    delay(10000);  // play sound for 10 seconds
    digitalWrite(SOUND, LOW);
    delay(20000); // wait for 20 seconds */
    println("Ding ding ding! This is the sound !!");
}

// create function to display medication details
void displayMedication(int compartment) {
    println("Ding ding ding! It's time to take your medication !! On today's menu, we have ");
    print(schedule[compartment].name);
    printf(" : ");
    printfln(schedule[compartment].effect);
}

// create function to calculate rotation angle
int calc(int i) {
    return ANGLE * schedule[i].compartment; //compartments evenly spaced;
}

// create function to rotate servo motor
void rotateServo(int angle) {
    /** myServo.write(angle);
    if (checkMedicationStatus() == 1){
        myServo.write(0);  // reset servo
    } */
   println ("Rotating servo to an angle of: %d", angle);
}

// initalize system
/** void setup() {
    pinMode(SOUND, OUTPUT);
    myServo.attach(SERVO);
    
    Serial.begin(9600); 
} */

// main code 
void loop() {
    char input[BUFFER];  // stores incoming serial data from Python script

    // only runs if data is available in buffer
    // if (Serial.available()) {
        int index = 0;

        readSerialData(input);

        storeSchedule(input);
    // }

    // Check if it's time for any medication
    if (timeSet == 1) { // only once device is calibrated
        checkTime();
    }
}
