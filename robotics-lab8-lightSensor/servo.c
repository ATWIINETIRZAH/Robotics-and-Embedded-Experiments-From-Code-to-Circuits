

#include <Servo.h>

// Define pins for the servo motor and LEDs
const int servoPin = A2;
const int led1Pin = A3;
const int led2Pin = A4;

// Define pin for the light sensor
const int lightSensorPin = A5;

// Create a Servo object
Servo servo;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Attach the servo motor to the servo pin
  servo.attach(servoPin);

  // Set the LED pins as outputs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  // Set initial position of the servo motor to 0 degrees
  servo.write(0);

  // Turn off LEDs initially
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
}

void loop() {
  // Read the value from the light sensor
  int lightValue = analogRead(lightSensorPin);
  Serial.print("Light Sensor Value: ");
  Serial.println(lightValue);

  // Check if the light sensor value is below a certain threshold
  if (lightValue <100) { // Adjust the threshold value as needed
    // Move the servo motor to 180 degrees
    servo.write(180);

    // Turn on LEDs
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);

    // Wait for a short time
    delay(1000);

    // Move the servo motor back to 0 degrees
    servo.write(0);

    // Turn off LEDs
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
  }

  // Wait before the next loop
  delay(100);
}
