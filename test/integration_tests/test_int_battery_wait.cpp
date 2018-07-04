/* This test exercises the WaitForBatteries method with simulated batteries */

#include "../../src/power/gb_abstract_battery.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>

class FakeBattery : public GbAbstractBattery {
private:
  char _scheme;

public:
  FakeBattery(char scheme)
      : GbAbstractBattery(1, 3.4, 3.5, 3), _scheme(scheme){};
  float GetVoltage() {
    float voltage;
    switch (_scheme) {
    case 's':
      voltage = (sin(millis() / 10000.0) / 2.0) + 3.7;
      break;
    case 'a':
      voltage = (sin(millis() / 5000.0) / 2.0) + 3.7;
      break;
    default:
      voltage = 3.89;
      break;
    }
    return voltage;
  }
};

static const int BATTERY_WAIT_TIME = 10;
FakeBattery battery1 = FakeBattery('s');
FakeBattery battery2 = FakeBattery('a');

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Wait_for_batteries test starting..."));
}

void loop() {
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  DEBUG_PRINT("battery1 voltage: ");
  DEBUG_PRINT(battery1.GetVoltage());
  DEBUG_PRINT(" | battery1 status: ");
  DEBUG_PRINTLN(battery1.Status());
  DEBUG_PRINT("battery2 voltage: ");
  DEBUG_PRINT(battery2.GetVoltage());
  DEBUG_PRINT(" | battery2 status: ");
  DEBUG_PRINTLN(battery2.Status());
  DEBUG_PRINTLN("*******************************************");
  delay(500);
}
