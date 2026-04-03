#include <Arduino.h>
#define LED_BUILTIN 2

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello from ESP32 Setup");
}

void loop()
{
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Hello from ESP32 Loop");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}