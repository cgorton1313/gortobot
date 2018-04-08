#include <Arduino.h>
#include "configs/includes.h"

GbBattery battery = GbBattery(1, 16, 3.4, 3.5, 4, true);
GbBlinker blinker = GbBlinker(13);

void setup() { Serial.begin(115200); }

void loop() {
  Serial.print(F("Voltage = "));
  Serial.print(battery.GetVoltage());
  Serial.print(F(" | Status = "));
  Serial.println(battery.Status());
  blinker.Blink(2);
  delay(500);
}
