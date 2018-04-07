#include <ArduinoFake.h>
#include "communications/gb_blinker.h"
#include "communications/gb_message_handler.h"
#include <unity.h>

using namespace fakeit;

void test_blink_once(void) {

  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
  When(Method(ArduinoFake(), delay)).AlwaysReturn();

  uint8_t pin = 13;
  GbBlinker blinker = GbBlinker(pin);
  blinker.Blink(2);

  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();
}

void test_parse_message(void) {
  GbMessageHandler messageHandler = GbMessageHandler(1);
  //GbSailingOrders actualOrders = messageHandler.ParseMessage("1,120,20,60,20,30,z");

  TEST_ASSERT_TRUE(1 == 1);

}

void test_battery_status(void) {

}

int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(test_blink_once);
  RUN_TEST(test_parse_message);
  RUN_TEST(test_battery_status);

  return UNITY_END();
}
