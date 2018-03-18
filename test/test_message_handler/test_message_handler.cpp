#include "..\src\communications\gb_message_handler.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_message_handler(void) {
  GbMessageHandler messageHandler = GbMessageHandler();
  TEST_ASSERT_EQUAL(2, messageHandler.GetDiagnosticMessage());
}

void setup() {
  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_message_handler);

  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }

#endif
