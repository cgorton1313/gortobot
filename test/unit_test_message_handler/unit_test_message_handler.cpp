#include "..\src\communications\gb_message_handler.h"
#include "..\src\communications\gb_sailing_orders.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

void test_BuildOutboundMessage(void) {
  String outboundMessage = "";
  String expected = "";
  GbMessageHandler messageHandler = GbMessageHandler();
  GbFix fix = {.latitude = 0.0,
               .longitude = 0.0,
               .year = 1972,
               .month = 1,
               .day = 1,
               .hour = 1,
               .minute = 1,
               .second = 1,
               .satellites = 1};

  // Test version 4 message (long form 2 batteries)
  outboundMessage =
      messageHandler.BuildOutboundMessage(4, 1, 1, fix, 3.99, 3.99, 180, 0);
  Serial.println(outboundMessage);
  expected = "4,1,1,720101010101,0.0000,0.0000,3.99,3.99,180,0";
  TEST_ASSERT_TRUE(outboundMessage == expected);

  // Test version 5 message (base62 form 2 batteries)
  outboundMessage =
      messageHandler.BuildOutboundMessage(5, 1, 1, fix, 3.99, 3.99, 180, 0);
  Serial.println(outboundMessage);
  expected = "5,1,1,1A11111,3m88,7YGG,6R,6R,2u,0";
  TEST_ASSERT_TRUE(outboundMessage == expected);
}

void test_GetDiagnosticMessage(void) {
  GbMessageHandler messageHandler = GbMessageHandler();
  TEST_ASSERT_EQUAL(0, messageHandler.GetDiagnosticMessage());
}

void test_ParseMessage_validMessage(void) {
  static GbSailingOrders validSailingOrders = {.loggingInterval = 60,
                                               .orderedSailPositionA = 270,
                                               .orderedTackTimeA = 90,
                                               .orderedSailPositionB = 270,
                                               .orderedTackTimeB = 90};

  GbMessageHandler messageHandler = GbMessageHandler();
  String testMessage = "1,180,1,270,2,5,z";
  GbSailingOrders parsedOrders =
      messageHandler.ParseMessage(testMessage, validSailingOrders);

  bool success = false;
  if (parsedOrders.orderedSailPositionA == 180 &&
      parsedOrders.orderedTackTimeA == 1 &&
      parsedOrders.orderedSailPositionB == 270 &&
      parsedOrders.orderedTackTimeB == 2 && parsedOrders.loggingInterval == 5) {
    success = true;
  }

  TEST_ASSERT_TRUE(success);
}

void test_ParseMessage_invalidMessage(void) {
  static GbSailingOrders validSailingOrders = {.loggingInterval = 60,
                                               .orderedSailPositionA = 270,
                                               .orderedTackTimeA = 90,
                                               .orderedSailPositionB = 270,
                                               .orderedTackTimeB = 90};

  GbMessageHandler messageHandler = GbMessageHandler();
  GbSailingOrders parsedOrders;

  // Test no 'z'
  parsedOrders =
      messageHandler.ParseMessage("1,180,1,270,2,5", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);

  // Test >359 angle
  parsedOrders =
      messageHandler.ParseMessage("1,360,1,270,2,5,z", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);

  // Test <0 angle
  parsedOrders =
      messageHandler.ParseMessage("1,180,1,-1,2,5,z", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);

  // Test >12 hour orderedTackTime
  parsedOrders =
      messageHandler.ParseMessage("1,180,44000,270,2,5,z", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);

  // Test >1 day loggingInterval
  parsedOrders =
      messageHandler.ParseMessage("1,180,1,270,2,90000,z", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);

  // Test invalid message type
  parsedOrders =
      messageHandler.ParseMessage("2,180,1,270,2,5,z", validSailingOrders);
  TEST_ASSERT_EQUAL(60, parsedOrders.loggingInterval);
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
