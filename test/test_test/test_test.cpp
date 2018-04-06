#include <ArduinoFake.h>
#include "communications/gb_blinker.h"
#include <unity.h>

using namespace fakeit;

void test_blink(void)
{

  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
  When(Method(ArduinoFake(), delay)).AlwaysReturn();

  uint8_t pin = 13;
  GbBlinker blinker = GbBlinker(pin);
  blinker.Blink(2);

  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();

}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_blink);

    UNITY_END();

    return 0;
}
