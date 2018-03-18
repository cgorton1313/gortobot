#include "gb_sentence_builder.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_string_builder(void) {
  GbSentenceBuilder sentence_builder = GbSentenceBuilder(2);
  GbFix fix = {.latitude = 0.0,
               .longitude = 0.0,
               .year = 1972,
               .month = 1,
               .day = 1,
               .hour = 1,
               .minute = 1,
               .second = 1,
               .satellites = 1};

  String logSentence = sentence_builder.Sentence(1, 1, fix, 3.99, 3.99, 180, 0);
  String expected = "2,1,1,720101010101,0.0000,0.0000,3.99,180,0";
  bool success;
  if (logSentence == expected) {
    success = true;
  }

  TEST_ASSERT_TRUE(success);
}

void setup() {
  UNITY_BEGIN();

  RUN_TEST(test_string_builder);

  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }

#endif
