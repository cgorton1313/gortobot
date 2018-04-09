#include "..\src\communications\gb_message_handler.h"
#include "..\src\communications\gb_sailing_orders.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

void test_BuildOutboundMessage(void) {
  GbMessageHandler messageHandler = GbMessageHandler(2);
  GbFix fix = {.latitude = 0.0,
               .longitude = 0.0,
               .year = 1972,
               .month = 1,
               .day = 1,
               .hour = 1,
               .minute = 1,
               .second = 1,
               .satellites = 1};

  String logSentence =
      messageHandler.BuildOutboundMessage(1, 1, fix, 3.99, 3.99, 180, 0);
  String expected = "2,1,1,720101010101,0.0000,0.0000,3.99,180,0";
  bool success;
  if (logSentence == expected) {
    success = true;
  }

  TEST_ASSERT_TRUE(success);
}

void test_GetDiagnosticMessage(void) {
  GbMessageHandler messageHandler = GbMessageHandler(2);
  TEST_ASSERT_EQUAL(2, messageHandler.GetDiagnosticMessage());
}

void test_ParseMessage_validMessage(void) {
  GbMessageHandler messageHandler = GbMessageHandler(2);
  String testMessage = "1,180,1,270,2,5,z";
  GbSailingOrders parsedOrders = messageHandler.ParseMessage(testMessage);

  bool success = false;
  if (parsedOrders.orderedSailPositionA == 180 &&
      parsedOrders.orderedTackTimeA == 1 &&
      parsedOrders.orderedSailPositionB == 270 &&
      parsedOrders.orderedTackTimeB == 2 &&
      parsedOrders.loggingInterval == 5) {
    success = true;
  }

  TEST_ASSERT_TRUE(success);
}


void setup() {
  delay(1000); // for proper printing
  UNITY_BEGIN();

  RUN_TEST(test_BuildOutboundMessage);
  RUN_TEST(test_GetDiagnosticMessage);
  RUN_TEST(test_ParseMessage_validMessage);

  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }

#endif
