

// use the libraryfor the servo motors
#include <Servo.h>

// Define pins for ultrasonic sensor
const int trigPin = 9;
const int echoPin = 8;

// Define pin for relay and servo
const int relayPin = 7;
Servo myServo;

long duration;
int distance;
int servoPosition = 90;  // Default position of the servo

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);

  // Set ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set relay pin as output
  pinMode(relayPin, OUTPUT);
  
  // Attach servo to pin 3
  myServo.attach(3);
  myServo.write(servoPosition);  // Set initial position

  // Initially deactivate relay (LOW)
  digitalWrite(relayPin, LOW);
}

void loop() {
  // Read distance from the ultrasonic sensor
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // If distance is less than 20 cm, activate relay and move servo
  if (distance > 0 && distance <= 20) {
    // Activate relay
    digitalWrite(relayPin, HIGH);
    delay(100);  // Wait for relay to stabilize

    // Move the servo to a new position
    servoPosition = 0;
    myServo.write(servoPosition);
    Serial.println("Servo Activated");
  } else {
    // Deactivate relay
    digitalWrite(relayPin, LOW);
    delay(100);  // Wait for relay to stabilize

    // Move servo to default position
    servoPosition = 90;
    myServo.write(servoPosition);
    Serial.println("Servo Deactivated");
  }

  // Small delay before next reading
  delay(500);
}

// Function to get distance from ultrasonic sensor
int getDistance() {
  // Send a 10us pulse to trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo pin and calculate distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to centimeters

  return distance;
}
