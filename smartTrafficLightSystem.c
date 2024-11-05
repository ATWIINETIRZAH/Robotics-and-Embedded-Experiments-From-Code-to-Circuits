
#include <LiquidCrystal.h>
#include <Servo.h>

// Initialize the library with the numbers of the interface pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define pins for the LEDs
const int redLed = 6;
const int yellowLed = 7;
const int greenLed = 8;
const int pedestrianLed = 13; // LED for pedestrian crossing

// Define pins for the buzzer and pedestrian button
const int buzzerPin = 9;
const int pedestrianButtonPin = 10;

// Define pins for the ultrasonic sensor
const int trigPin = A0;
const int echoPin = A1;

const int led1Pin = A3;
const int led2Pin = A4;

// Define light durations in seconds
const int greenDuration = 10;
const int yellowDuration = 3;
const int redDuration = 10;
const int pedestrianDuration = 10; // Pedestrian crossing duration

// Enum for traffic light states
enum TrafficLightState { RED, RED_TO_YELLOW, GREEN, YELLOW, PEDESTRIAN, PEDESTRIAN_TO_YELLOW };
TrafficLightState currentLightState = RED;

// Timer variables
unsigned long previousMillis = 0;
int timer = redDuration;

// Create a Servo object
Servo servo;

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.clear(); // Clear the display

  // Set the LED pins as outputs
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(pedestrianLed, OUTPUT);

  // Set the LED pins as outputs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  // Set the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);

  // Set the pedestrian button pin as an input
  pinMode(pedestrianButtonPin, INPUT_PULLUP);

  // Set the ultrasonic sensor pins as outputs and inputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Attach the servo motor to the servo pin
  servo.attach(A2);

  // Set initial position of the servo motor to 0 degrees
  servo.write(0);

  // Display initial status on the LCD
  lcd.setCursor(0, 0); // Set cursor to the first row
  lcd.print("STOP");
  lcd.setCursor(0, 1); // Set cursor to the second row
  lcd.print("Time: 10s");
  timer = redDuration;

  // Set initial LED states
  digitalWrite(redLed, HIGH);
  digitalWrite(yellowLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(pedestrianLed, LOW);

  // Turn off LEDs initially
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
}

void loop() {
  // Get the current time in milliseconds
  unsigned long currentMillis = millis();

  // Read the value from the light sensor
  int lightValue = analogRead(A5);
  Serial.print("Light Sensor Value: ");
  Serial.println(lightValue);

  // Check if the light sensor value is below a certain threshold
  if (lightValue < 100) { // Adjust the threshold value as needed
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

  // Check if the pedestrian button is pressed
  if (digitalRead(pedestrianButtonPin) == LOW && currentLightState != PEDESTRIAN && currentLightState != PEDESTRIAN_TO_YELLOW) {
    // Activate the buzzer for 1 second
    tone(buzzerPin, 1000);
    delay(1000);
    noTone(buzzerPin);

    // Change the light state to allow pedestrians to cross
    currentLightState = PEDESTRIAN;
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(pedestrianLed, HIGH);
    timer = pedestrianDuration;
    previousMillis = currentMillis; // Reset timer
    lcd.setCursor(0, 0);
    lcd.print("PEDESTRIAN "); // Clear remaining characters with spaces
  }

  // Check if it's time to change the light
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    timer--;

    // Change the light state based on the timer
    if (timer <= 0) {
      switch (currentLightState) {
        case RED:
          currentLightState = RED_TO_YELLOW;
          timer = yellowDuration;
          lcd.setCursor(0, 0);
          lcd.print("READY     "); // Clear remaining characters with spaces
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, HIGH);
          digitalWrite(greenLed, LOW);
          break;
        case RED_TO_YELLOW:
          currentLightState = GREEN;
          timer = greenDuration;
          lcd.setCursor(0, 0);
          lcd.print("GO       "); // Clear remaining characters with spaces
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, HIGH);
          break;
        case GREEN:
          currentLightState = YELLOW;
          timer = yellowDuration;
          lcd.setCursor(0, 0);
          lcd.print("WAIT     "); // Clear remaining characters with spaces
          digitalWrite(greenLed, LOW);
          digitalWrite(yellowLed, HIGH);
          break;
        case YELLOW:
          currentLightState = RED;
          timer = redDuration;
          lcd.setCursor(0, 0);
          lcd.print("STOP     "); // Clear remaining characters with spaces
          digitalWrite(yellowLed, LOW);
          digitalWrite(redLed, HIGH);
          break;
        case PEDESTRIAN:
          currentLightState = PEDESTRIAN_TO_YELLOW;
          timer = yellowDuration;
          lcd.setCursor(0, 0);
          lcd.print("READY     "); // Clear remaining characters with spaces
          digitalWrite(pedestrianLed, LOW);
          digitalWrite(yellowLed, HIGH);
          digitalWrite(redLed, LOW);
          break;
        case PEDESTRIAN_TO_YELLOW:
          currentLightState = GREEN;
          timer = greenDuration;
          lcd.setCursor(0, 0);
          lcd.print("GO       "); // Clear remaining characters with spaces
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, HIGH);
          break;
      }
    }
  }

  // Display timer on the LCD
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(timer);
  lcd.print("s ");

  // Adjust light durations based on ultrasonic sensor distance
  long distance = readUltrasonicDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= 20) { // Change the threshold distance as needed
    // For close distances, reduce the green light duration
    if (currentLightState == GREEN) {
      timer = min(timer, 5); // Reduce green light duration to 5 seconds if close distance
    }
  } else {
    // For other distances, use default durations
    if (currentLightState == GREEN) {
      timer = min(timer, greenDuration); // Use default green light duration
    }
  }

  // Wait for a short time before the next loop
  delay(100);
}

// Function to measure distance using ultrasonic sensor
long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert duration to distance
  return distance;
}