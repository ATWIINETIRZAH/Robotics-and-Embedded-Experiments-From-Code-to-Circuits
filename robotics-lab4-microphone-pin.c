
#include <Servo.h>  // Include the Servo library

int ledPin = 10;
int led = 11;
int microphonePin = 1;
int state = 0;
Servo myServo;  // Create a Servo object
int servoPin = 12;  // Connect the servo signal pin to digital pin 10

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(microphonePin, INPUT);
  
  myServo.attach(servoPin);  // Attach the servo to the specified pin
  myServo.write(0);  // Initial position of the servo (0 degrees)
}

void loop() {
  state = digitalRead(microphonePin);

  if (state == HIGH) {
    digitalWrite(ledPin, HIGH);  // Turn on the LED connected to ledPin
    digitalWrite(led, LOW);      // Turn off the other LED
    myServo.write(90);           // Rotate the servo to 90 degrees
    delay(1000);                 // Wait for 1 second
    myServo.write(0);            // Rotate the servo back to 0 degrees
    delay(1000);                 // Wait for 1 second
  } else {
    digitalWrite(ledPin, LOW);   // Turn off the LED connected to ledPin
    digitalWrite(led, HIGH);     // Turn on the other LED
  }
}