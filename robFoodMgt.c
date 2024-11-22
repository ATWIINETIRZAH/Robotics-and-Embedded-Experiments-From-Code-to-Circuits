#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>


#define SS_PIN 11
#define RST_PIN 12

MFRC522 rfid(SS_PIN, RST_PIN);        // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Initialize the LCD with I2C address 0x27
RTC_DS1307 rtc;                       // Create RTC instance for DS1307

// Keypad settings
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define user data structure
struct User {
  byte uid[4];
  char name[16];
  char pin[4];
  float balance;
};

// Array of registered users (stored in EEPROM)
User users[] = {
  {{0x03, 0x9C, 0x3B, 0x1A}, "Daniel W", "1234", 50.0},
  {{0x73, 0xA4, 0xC3, 0x95}, "James Alala", "5678", 30.0},
  {{0xBC, 0x03, 0x37, 0xBB}, "SETH", "9012", 20.0},
  {{0x60, 0xEA, 0x21, 0x12}, "TIRZAH", "3456", 40.0}
};

const byte numOfUsers = sizeof(users) / sizeof(users[0]);
int failedAttempts = 0;
bool blocked = false;
unsigned long blockStartTime;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(2000);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");

  // Initialize users in EEPROM (run once, then comment out)
  initializeUsers();
}

void loop() {
  if (blocked) {
    if (millis() - blockStartTime >= 30000) {
      blocked = false;
      failedAttempts = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place your card");
    }
    return;
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card detected");

  User user;
  bool found = getUserFromEEPROM(rfid.uid.uidByte, user);

  if (found) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN:");

    String enteredPin = "";
    while (enteredPin.length() < 4) {
      char key = keypad.getKey();
      if (key) {
        enteredPin += key;
        lcd.setCursor(enteredPin.length(), 1);
        lcd.print('*');
      }
    }

    if (enteredPin == String(user.pin)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("1: Check Bal");
      lcd.setCursor(0, 1);
      lcd.print("2: Buy Meal");

      char option = 0;
      while (!option) {
        char key = keypad.getKey();
        if (key == '1' || key == '2') {
          option = key;
        }
      }

      if (option == '1') {
        // Balance Check Mode
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Balance:");
        lcd.setCursor(0, 1);
        lcd.print(user.balance, 2); // Display balance with 2 decimal places
        Serial.print("Balance Check: ");
        Serial.println(user.balance, 2);
        delay(5000); // Show balance for 5 seconds
      } else if (option == '2') {
        // Deduction Mode
        float cost = 5.0; // Deduction amount
        if (user.balance >= cost) {
          user.balance -= cost;
          updateUserInEEPROM(user);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Thank you,");
          lcd.setCursor(0, 1);
          lcd.print(user.name);
          Serial.print("Transaction Successful. New Balance: ");
          Serial.println(user.balance);
          delay(3000);
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Insufficient");
          lcd.setCursor(0, 1);
          lcd.print("Balance!");
          Serial.println("Transaction Failed: Insufficient Balance");
          delay(3000);
        }
      }
    } else {
      failedAttempts++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect PIN");
      if (failedAttempts >= 3) {
        blocked = true;
        blockStartTime = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Blocked for 30s");
      }
      delay(2000);
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    delay(2000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// Check if UID matches
bool checkUID(byte *knownUID) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != knownUID[i]) {
      return false;
    }
  }
  return true;
}

// Update balance in EEPROM
void updateUserInEEPROM(User &user) {
  for (byte i = 0; i < numOfUsers; i++) {
    if (strcmp(users[i].name, user.name) == 0) {
      int startAddr = i * sizeof(User);
      EEPROM.put(startAddr, user); // Update user data in EEPROM
      break;
    }
  }
}

// Initialize user data in EEPROM (run once, then comment out)
void initializeUsers() {
  for (byte i = 0; i < numOfUsers; i++) {
    int startAddr = i * sizeof(User);
    EEPROM.put(startAddr, users[i]); // Store user data in EEPROM
  }
}

// Retrieve user data from EEPROM
bool getUserFromEEPROM(byte *uid, User &user) {
  for (byte i = 0; i < numOfUsers; i++) {
    int startAddr = i * sizeof(User);
    User temp;
    EEPROM.get(startAddr, temp);
    if (memcmp(temp.uid, uid, 4) == 0) {
      user = temp;
      return true;
    }
  }
  return false;
}
