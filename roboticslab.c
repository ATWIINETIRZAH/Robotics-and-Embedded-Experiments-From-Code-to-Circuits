
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RTClib.h>

#define SS_PIN 11
#define RST_PIN 12

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

struct User {
  byte uid[4];
  String name;
  String pin;
};

User users[] = {
  {{0x03, 0x9C, 0x3B, 0x1A}, "Daniel W", "1234"},
  {{0x73, 0xA4, 0xC3, 0x95}, "James Alala", "5678"},
  {{0xBC, 0x03, 0x37, 0xBB}, "SETH", "9012"},
  {{0x60, 0xEA, 0x21, 0x12}, "TIRZAH", "3456"}
};

const byte numOfUsers = sizeof(users) / sizeof(users[0]);

void setup() {
  Serial.begin(9600);          // Debugging for Mega Serial Monitor
  Serial1.begin(9600);         // Serial communication to ESP32
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card detected");

  String userName = "Unknown";
  bool accessGranted = false;

  for (byte i = 0; i < numOfUsers; i++) {
    if (checkUID(users[i].uid)) {
      accessGranted = true;
      userName = users[i].name;
      
      // Send the username and UID to the ESP32
      Serial1.print("User: ");
      Serial1.print(userName);
      Serial1.print(", UID: ");
      for (byte j = 0; j < 4; j++) {
        Serial1.print(rfid.uid.uidByte[j], HEX);
        if (j < 3) Serial1.print(":");
      }
      Serial1.println();

      break;
    }
  }

  if (accessGranted) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome, " + userName);
    delay(2000);
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

bool checkUID(byte *knownUID) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != knownUID[i]) {
      return false;
    }
  }
  return true;
}