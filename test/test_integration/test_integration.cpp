#include <Arduino.h>
#include <gb_library.h>

GbBattery battery = GbBattery(1, 16, 3.4, 3.5, 4, true);
//GbBlinker blinker = GbBlinker(13);

void setup() { Serial.begin(115200); }

void loop() {
  Serial.print("voltage = ");
  Serial.print(battery.GetVoltage());
  Serial.print(", status = ");
  Serial.println(battery.Status());
  //blinker.Blink(2);
  delay(1000);
}
