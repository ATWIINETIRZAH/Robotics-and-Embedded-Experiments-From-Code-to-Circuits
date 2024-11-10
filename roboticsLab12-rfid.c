

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 7
#define GREEN_LED_PIN 4   // Green LED pin
#define RED_LED_PIN 5     // Red LED pin

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

   
// Define the correct card UID (replace with your actual card UID)
byte correctCardUID[] = {0x73, 0xA4, 0xC3, 0x95}; // Example UID

void setup() {
  Serial.begin(9600);         // Start the serial communication at 9600 baud rate
  SPI.begin();                // Start the SPI bus
  rfid.PCD_Init();            // Initialize the MFRC522 reader
  
  // Set the LED pins as outputs
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // Turn off both LEDs initially
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  Serial.println("Scan an RFID card to get its UID...");
}

void loop() {
  // Check if a new RFID card is present
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Read the card's serial number (UID)
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print the UID of the card to the serial monitor
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println(); // Add a new line after the UID

  // Check if the scanned card is the correct one
  if (checkUID(rfid.uid.uidByte, rfid.uid.size)) {
    // Correct card detected
    Serial.println("Correct Card! Access Granted.");
    digitalWrite(GREEN_LED_PIN, HIGH);  // Turn on green LED
    delay(3000);                        // Keep green LED on for 3 seconds
    digitalWrite(GREEN_LED_PIN, LOW);   // Turn off green LED
  } else {
    // Incorrect card detected
    Serial.println("Incorrect Card! Access Denied.");
    digitalWrite(RED_LED_PIN, HIGH);    // Turn on red LED
    delay(3000);                        // Keep red LED on for 3 seconds
    digitalWrite(RED_LED_PIN, LOW);     // Turn off red LED
  }

  // Halt the card after reading the UID
  rfid.PICC_HaltA();
}

// Routine to compare the scanned UID with the correct one
bool checkUID(byte *buffer, byte bufferSize) {
  if (bufferSize != sizeof(correctCardUID)) return false; // If sizes don't match
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] != correctCardUID[i]) return false;
  }
  return true;
}