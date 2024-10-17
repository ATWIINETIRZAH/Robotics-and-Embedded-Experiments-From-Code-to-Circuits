

// define the led pin connections
#define LED_PIN_1 8
#define LED_PIN_2 9
#define LED_PIN_3 10

// determine whether they are output or input 
void setup()
{
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
}
void loop()
// when led_pin_1 is high, the rest are low
{
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  // hold for one second
  delay(1000);
// when led_pin_2 is high, the rest are low
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, HIGH);
  digitalWrite(LED_PIN_3, LOW);
  delay(1000);
// when led_pin_2 is high, the rest are low
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, HIGH);
  delay(1000);
}