#include <Arduino.h>

void setup() {}

void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }
