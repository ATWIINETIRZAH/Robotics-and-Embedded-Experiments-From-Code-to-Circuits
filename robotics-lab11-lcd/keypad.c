


#include <LiquidCrystal.h> 
#include <Keypad.h>

#define Password_Length 8 

int signalPin = 12;

char Data[Password_Length]; 
char Master[Password_Length] = "1257840"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

int led = 10;
int potPin = A5;  // Potentiometer pin to control LCD brightness
int brightness;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Define LCD pin connections (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(13, 11, A0, A1, A2, A3);

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  lcd.begin(16, 2);  // Initialize the 16x2 LCD
  pinMode(signalPin, OUTPUT);
  pinMode(led, OUTPUT);
  analogWrite(led, 255);  // Set initial brightness to maximum
}

void loop() {
  brightness = analogRead(potPin);  // Read the potentiometer value
  brightness = map(brightness, 0, 1023, 0, 255);  // Map the value to a PWM range of 0-255
  analogWrite(led, brightness);  // Adjust the backlight brightness

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count] = customKey; 
    lcd.setCursor(data_count, 1); 
    lcd.print(Data[data_count]); 
    data_count++; 
  }

  if (data_count == Password_Length - 1) {
    lcd.clear();

    if (!strcmp(Data, Master)) {
      lcd.print("Correct");

      digitalWrite(signalPin, HIGH); 
      digitalWrite(led, HIGH); 
      delay(5000);
      digitalWrite(signalPin, LOW);
      digitalWrite(led, LOW);
    }
    else {
      lcd.print("Incorrect");
      delay(1000);
    }

    lcd.clear();
    clearData();  
  }
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0; 
  }
  return;
}