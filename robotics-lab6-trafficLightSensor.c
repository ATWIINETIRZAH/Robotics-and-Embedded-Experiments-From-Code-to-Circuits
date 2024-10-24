

// Define pins for LEDs and push button

const int redLight = 9;
const int yellowLight = 10;
const int greenLight = 11;
const int pedestrianButton = 2;
const int pedestrianLight = 12;

void setup() {
  // Initialize the pins as outputs or inputs
  pinMode(redLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(greenLight, OUTPUT);
  pinMode(pedestrianLight, OUTPUT);
  pinMode(pedestrianButton, INPUT_PULLUP);

  // Start with green light on
  digitalWrite(greenLight, HIGH);
  digitalWrite(yellowLight, LOW);
  digitalWrite(redLight, LOW);
  digitalWrite(pedestrianLight, LOW);
}

void loop() {
  // Check if the pedestrian button is pressed
  if (digitalRead(pedestrianButton) == LOW) {
    // Turn the green light off and yellow light on for a short time
    digitalWrite(greenLight, LOW);
    digitalWrite(yellowLight, HIGH);
    delay(2000); // Wait for 2 seconds

    // Turn yellow light off and red light on
    digitalWrite(yellowLight, LOW);
    digitalWrite(redLight, HIGH);
    delay(2000); // Wait for 2 seconds to ensure cars stop

    // Turn on pedestrian light to indicate they can cross
    digitalWrite(pedestrianLight, HIGH);
    delay(5000); // Pedestrian crossing time

    // Turn off pedestrian light
    digitalWrite(pedestrianLight, LOW);

    // Turn red light off and yellow light on for a short time
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, HIGH);
    delay(2000); // Wait for 2 seconds

    // Turn yellow light off and green light on
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, HIGH);
  }
}
