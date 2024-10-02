#include <VARSTEP_ultrasonic.h>   // Include the custom VARSTEP_ultrasonic library for ultrasonic sensor control
#include "SoftwareSerial.h"       // Include the SoftwareSerial library for software-based serial communication
#include "DFRobotDFPlayerMini.h"  // Include the DFRobotDFPlayerMini library to control the DFPlayer Mini module

#define trigger_pin 8             // Define the pin connected to the ultrasonic sensor's trigger pin
#define echo_pin 9                // Define the pin connected to the ultrasonic sensor's echo pin

double distance_cm, distance_m;   // Declare variables to store the measured distance in centimeters and meters

VARSTEP_ultrasonic my_HCSR04(trigger_pin, echo_pin);  // Create an instance of the ultrasonic sensor with the specified trigger and echo pins

SoftwareSerial mySoftwareSerial(2, 3); // RX, TX     // Create a software serial instance for communicating with the DFPlayer Mini on pins 2 (RX) and 3 (TX)
DFRobotDFPlayerMini myDFPlayer;                      // Create an instance of the DFPlayer Mini module

unsigned long previousMillis = 0;       // Variable to store the previous time for track playback control
unsigned long loopDelayMillis = 0;      // Variable to store the previous time for controlling the loop delay
const long interval = 2000;             // Define a time interval of 2000 milliseconds (2 seconds) for track playback
const long loopDelayInterval = 100;     // Define a time interval of 100 milliseconds for replacing the delay function
bool trackPlayed = false;               // Boolean variable to track if a sound has been played

void setup() {
  mySoftwareSerial.begin(9600);         // Start software serial communication at 9600 baud rate
  Serial.begin(115200);                 // Start serial communication at 115200 baud rate for debugging

  while (!Serial);                      // Wait for the serial communication to initialize

  if (!myDFPlayer.begin(mySoftwareSerial)) {  // Initialize the DFPlayer Mini module with software serial
    Serial.println(F("Not initialized:"));   // Print error message if initialization fails
    Serial.println(F("1. Check the DFPlayer Mini connections"));  // Provide suggestions to fix the issue
    Serial.println(F("2. Insert an SD card"));
    while (true);                           // Halt the program if initialization fails
  }

  Serial.println(F("DFPlayer Mini module initialized!")); // Print a success message when the module is initialized
  myDFPlayer.setTimeOut(500);    // Set the timeout for serial communication to 500 milliseconds
  myDFPlayer.volume(30);         // Set the volume level of the DFPlayer Mini (range is 0-30)
  myDFPlayer.EQ(0);              // Set the equalizer (0: Normal, 1: Pop, 2: Rock, 3: Jazz, 4: Classic, 5: Bass)
}

void loop() {
  unsigned long currentMillis = millis();  // Store the current time in milliseconds

  if (currentMillis - loopDelayMillis >= loopDelayInterval) {  // Check if the loop delay interval has passed
    loopDelayMillis = currentMillis;      // Update the loop delay timer

    distance_cm = my_HCSR04.distance_cm(); // Measure the distance in centimeters using the ultrasonic sensor
    distance_m = my_HCSR04.distance_m();   // Measure the distance in meters using the ultrasonic sensor

    if (distance_cm == -1.0) {            // Check if no distance is detected
      Serial.println("No distance detected"); // Print a message if no distance is detected
      trackPlayed = false;               // Reset the trackPlayed flag
    } else {
      if (!trackPlayed || currentMillis - previousMillis >= interval) { // Check if a track needs to be played
        previousMillis = currentMillis;  // Update the previousMillis timer

        // Play different tracks based on the detected distance range
        if (distance_cm <= 3.99 && distance_cm <= 3.80) {
          myDFPlayer.play(1);            // Play track 1 if the distance is less than or equal to 3.99 cm and greater than 3.80 cm
        } else if (distance_cm <= 5.99 && distance_cm >= 4.99) {
          myDFPlayer.play(2);            // Play track 2 if the distance is between 4.99 cm and 5.99 cm
        } else if (distance_cm <= 7.99 && distance_cm >= 6.99) {
          myDFPlayer.play(3);            // Play track 3 if the distance is between 6.99 cm and 7.99 cm
        } else if (distance_cm <= 9.9 && distance_cm >= 8.99 ) {
          myDFPlayer.play(4);            // Play track 4 if the distance is between 8.99 cm and 9.9 cm
        }

        trackPlayed = true;              // Set the trackPlayed flag to true after playing a track
      }
    }

    Serial.print("Distance: ");          // Print the detected distance
    Serial.print(distance_cm);           // Print the distance in centimeters
    Serial.print("cm | ");
    Serial.print(distance_m);            // Print the distance in meters
    Serial.println("m");
  }
}
