
// Define pin connections
const int trigPin = 8;
const int echoPin = 9;
const int leftMotorPin1 = 5;
const int leftMotorPin2 = 6;
const int rightMotorPin1 = 10;
const int rightMotorPin2 = 11;

// Define constants
const int safeDistance = 25; // Safe distance from obstacles in centimeters

void setup() {
  // Initialize pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
}

void loop() {
  // Send ultrasonic signal
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calculate distance
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // Check for obstacles
  if (distance < safeDistance) {
    // Obstacle detected, stop and turn
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
    delay(500);
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    delay(1000);
  } else {
    // No obstacle detected, move forward
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }
}

