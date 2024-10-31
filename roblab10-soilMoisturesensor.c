

int moisturePin = A0; // Pin to which the soil moisture sensor is connected

void setup() {
  Serial.begin(9600);
}

void loop() {
  int moistureVal = analogRead(moisturePin); // Read the analog value from the soil moisture sensor
  Serial.println(moistureVal); // Print the value to the serial monitor
  delay(1000); // Wait for 1 second before reading again
}
