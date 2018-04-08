#include "configs/includes.h"
#include "power/gb_abstract_battery.h"
#include "power/gb_battery.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

class FakeBattery : public GbAbstractBattery {
public:
  FakeBattery() : GbAbstractBattery(1, 3.4, 3.5, 3){};
  float GetVoltage() {
    return (sin(millis()/10000.0)/2.0) + 3.7;
  }
};

static const int BATTERY_WAIT_TIME = 4;
FakeBattery battery1 = FakeBattery();
FakeBattery battery2 = FakeBattery();

void setup() {
  Serial.begin(115200);
  Serial.println(F("Wait_for_batteries test starting..."));
}

void loop() {
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  Serial.print("battery1 voltage: ");
  Serial.print(battery1.GetVoltage());
  Serial.print(" | battery1 status: ");
  Serial.println(battery1.Status());
  Serial.print("battery2 voltage: ");
  Serial.print(battery2.GetVoltage());
  Serial.print(" | battery2 status: ");
  Serial.println(battery2.Status());
  Serial.println("*******************************************");
  delay(500);
}
