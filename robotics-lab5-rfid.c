

// include necessary libraries

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 //sda pin 
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);   // Start the serial communication at 9600 baud rate
  SPI.begin();          // Start the SPI bus
  rfid.PCD_Init();      // Initialize the MFRC522 reader
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

  // Halt the card after reading the UID
  rfid.PICC_HaltA();


}
