// library needed to display characters
#include <LiquidCrystal.h>

// Initialize the library by associating any needed LCD interface pin
const int rs = 13, en = 11, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Example: Hello, World");
}

void loop() {
}