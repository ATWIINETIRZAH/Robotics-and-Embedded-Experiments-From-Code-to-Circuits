



#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RTClib.h>            // RTC library for Tiny RTC DS1307/DS3231 module

#define SS_PIN 11
#define RST_PIN 12

MFRC522 rfid(SS_PIN, RST_PIN);        // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Initialize the LCD with I2C address 0x27 and size 16x2
RTC_DS1307 rtc;                       // Create RTC instance for DS1307

// Define keypad settings
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {10, 9, 8, 7};    // Connect keypad ROW0, ROW1, ROW2, ROW3
byte colPins[COLS] = {6, 5, 4, 3};    // Connect keypad COL0, COL1, COL2, COL3
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Structure to hold UID, associated name, and PIN code
struct User {
  byte uid[4];   // UID array of 4 bytes
  String name;   // Associated name
  String pin;    // Associated PIN
};

// Array of known users
User users[] = {
  {{0x03, 0x9C, 0x3B, 0x1A}, "Daniel W", "1234"},
  {{0x73, 0xA4, 0xC3, 0x95}, "James Alala", "5678"},
  {{0xBC, 0x03, 0x37, 0xBB}, "SETH", "9012"},
  {{0x60, 0xEA, 0x21, 0x12}, "TIRZAH", "3456"}
};

const byte numOfUsers = sizeof(users) / sizeof(users[0]);
int failedAttempts = 0;
bool blocked = false;
unsigned long blockStartTime;

void setup() {
  Serial.begin(9600);          // Start the serial communication at 9600 baud rate
  SPI.begin();                 // Start the SPI bus
  rfid.PCD_Init();             // Initialize the MFRC522 reader
  
  lcd.init();                  // Initialize the LCD
  lcd.backlight();             // Turn on the LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(1000);                 // Wait for a second before continuing

  // Initialize RTC and check if it’s connected
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to the date & time this sketch was compiled
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
}

void loop() {
  if (blocked) {
    // Check if 30 seconds have passed since blocking
    if (millis() - blockStartTime >= 30000) {
      blocked = false;
      failedAttempts = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place your card");
    }
    return; // Skip the rest of the loop if blocked
  }

  // Look for new RFID cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return; // If no card is detected, return
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card detected");

  String userName = "Unknown";
  String userPin = "";
  bool accessGranted = false;

  // Check if the card UID matches any known UIDs and retrieve the associated name and PIN
  for (byte i = 0; i < numOfUsers; i++) {
    if (checkUID(users[i].uid)) {
      accessGranted = true;
      userName = users[i].name;  // Get the name of the person
      userPin = users[i].pin;    // Get the associated PIN

      // Print the user name and timestamp on the serial monitor
      DateTime now = rtc.now();
      Serial.print("Card Scanned: ");
      Serial.print(userName);
      Serial.print(" at ");
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.println(now.second(), DEC);
      break;
    }
  }

  if (accessGranted) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN:");

    String enteredPin = "";
    while (enteredPin.length() < 4) {
      char key = keypad.getKey();
      if (key) {
        enteredPin += key;
        lcd.setCursor(enteredPin.length(), 1);
        lcd.print('*'); // Display asterisk for each key press
      }
    }

    if (enteredPin == userPin) {
      // Correct PIN entered
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thank you,");
      lcd.setCursor(0, 1);
      lcd.print(userName + "!");
      delay(2000); // Display message for 2 seconds
      failedAttempts = 0; // Reset failed attempts
    } else {
      // Incorrect PIN entered
      failedAttempts++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect PIN");

      if (failedAttempts >= 3) {
        // Block after 3 failed attempts
        blocked = true;
        blockStartTime = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Blocked for 30s");
      }

      delay(2000); // Display message for 2 seconds
    }
  } else {
    // Access denied
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    delay(2000); // Display message for 2 seconds
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");

  rfid.PICC_HaltA();           // Halt PICC
  rfid.PCD_StopCrypto1();      // Stop encryption on PCD
}

// Function to check if the scanned card UID matches a known UID
bool checkUID(byte *knownUID) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != knownUID[i]) {
      return false;  // If any byte does not match, return false
    }
  }
  return true;  // All bytes matched, return true
}
