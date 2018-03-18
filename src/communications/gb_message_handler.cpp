#include "gb_message_handler.h"

GbMessageHandler::GbMessageHandler(uint8_t message_version) {
  _message_version = message_version;
}

GbSailingOrders GbMessageHandler::ParseMessage(String inboundMessage) {
  GbSailingOrders newSailingOrders;
  // TODO deal with invalid messages
  bool rxMessageInvalid = false;

  if (inboundMessage.indexOf("z") >= 0) { // we should parse it
    switch ((inboundMessage.substring(0, inboundMessage.indexOf(',')))
                .toInt()) {            // get first value
    case 1: {                          // standard message = 1,120,20,60,20,30,z
      newSailingOrders.sailMode = 'r'; // real sail mode
      // find the commas, probably should move to char []?
      uint8_t firstCommaIndex = inboundMessage.indexOf(',');
      uint8_t secondCommaIndex =
          inboundMessage.indexOf(',', firstCommaIndex + 1);
      uint8_t thirdCommaIndex =
          inboundMessage.indexOf(',', secondCommaIndex + 1);
      uint8_t fourthCommaIndex =
          inboundMessage.indexOf(',', thirdCommaIndex + 1);
      uint8_t fifthCommaIndex =
          inboundMessage.indexOf(',', fourthCommaIndex + 1);
      uint8_t sixthCommaIndex =
          inboundMessage.indexOf(',', fifthCommaIndex + 1);
      String firstValue = inboundMessage.substring(0, firstCommaIndex);
      String secondValue =
          inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
      String thirdValue =
          inboundMessage.substring(secondCommaIndex + 1, thirdCommaIndex);
      String fourthValue =
          inboundMessage.substring(thirdCommaIndex + 1, fourthCommaIndex);
      String fifthValue =
          inboundMessage.substring(fourthCommaIndex + 1, fifthCommaIndex);
      String sixthValue =
          inboundMessage.substring(fifthCommaIndex + 1, sixthCommaIndex);
      // maybe restrict these values to 6 hr max and positive
      newSailingOrders.orderedSailPositionA =
          secondValue.toInt(); // tolerates spaces
      newSailingOrders.orderedTackTimeA = thirdValue.toInt();
      newSailingOrders.orderedSailPositionB = fourthValue.toInt();
      newSailingOrders.orderedTackTimeB = fifthValue.toInt();
      uint32_t tempLoggingInterval = sixthValue.toInt();
      if (tempLoggingInterval > 0 &&
          tempLoggingInterval <= 86400) { // 86,400 secs = 1 day
        newSailingOrders.loggingInterval = tempLoggingInterval;
      }
      break;
    }
    case 2: {                          // sail test mode 2,10,z is 10 min test
      newSailingOrders.sailMode = 't'; // test sail mode
      newSailingOrders.orderedSailPositionA = 180;
      newSailingOrders.orderedTackTimeA = 1;
      newSailingOrders.orderedSailPositionB = 180;
      newSailingOrders.orderedTackTimeB = 1;
      uint8_t firstCommaIndex = inboundMessage.indexOf(',');
      uint8_t secondCommaIndex =
          inboundMessage.indexOf(',', firstCommaIndex + 1);
      String secondValue =
          inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
      uint32_t tempLoggingInterval = secondValue.toInt();
      if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {
        newSailingOrders.loggingInterval = tempLoggingInterval;
      }
      break;
    }
    // TODO: implement this: directly setting ordered sail position not yet
    // supported
    // case 3: { // not to be used in real sailing
    //   sailMode = 's';
    //   uint8_t firstCommaIndex = inboundMessage.indexOf(',');
    //   uint8_t secondCommaIndex =
    //       inboundMessage.indexOf(',', firstCommaIndex + 1);
    //   String secondValue =
    //       inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
    //   //orderedSailPosition = secondValue.toInt();
    //   newSailingOrders.loggingInterval = 1; // will wait 1 second before
    //   asking again
    //   break;
    //}
    default: { Serial.println(F("Not a valid rx message type.")); }
    }
  } else {
    // How to return this?
    rxMessageInvalid = true;
    Serial.println(F("No valid message (no z in message)"));
  }

  Serial.print(F("Logging interval = "));
  Serial.println(newSailingOrders.loggingInterval);
  Serial.print(F("Sail order A = "));
  Serial.print(newSailingOrders.orderedSailPositionA);
  Serial.print(F(" for "));
  Serial.print(newSailingOrders.orderedTackTimeA);
  Serial.println(F(" minutes"));
  Serial.print(F("Sail order B = "));
  Serial.print(newSailingOrders.orderedSailPositionB);
  Serial.print(F(" for "));
  Serial.print(newSailingOrders.orderedTackTimeB);
  Serial.println(F(" minutes"));

  Serial.println(rxMessageInvalid); // just to clear warning, remove later
  return newSailingOrders;
}

String GbMessageHandler::GetSerialMessage() {
  Serial.println(F("Ready for orders:"));

  // Wait for the serial data
  while (!Serial.available()) {
  }

  return Serial.readString();
}

String GbMessageHandler::GetFakeMessage(GbSailingOrders sailingOrders) {
  //  1 (std)-
  //  mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z
  //  (end)
  String fakeOrderString = "";
  if (sailingOrders.sailMode == 't') {
    // Build string for direct setting sail position
    fakeOrderString = "3,";
    fakeOrderString += sailingOrders.loggingInterval;
    fakeOrderString += ",z";
  } else {
    // Build string for rotating sail from 0-360 each minute, with delay in
    // between of loggingInterval
    fakeOrderString = "1,0,1,360,1,";
    fakeOrderString += sailingOrders.loggingInterval;
    fakeOrderString += ",z";
  }
  return fakeOrderString;
}

uint8_t GbMessageHandler::GetDiagnosticMessage() {
  // 1- too many trim pulses and sail is moving
  // 2- sail not moving after trim pulses
  // 4- fram problem
  // 8- rxMessage not valid
  // add these up to determine which combo is being reported
  // TODO get trim result //return (trimRoutineExceededMax * pow(2, 0) +
  // sailNotMoving * pow(2, 1) +
  // rxMessageInvalid * pow(2, 3));
  return 2;
}

// TODO: use
// https://stackoverflow.com/questions/2462951/c-equivalent-of-stringbuffer-stringbuilder
String GbMessageHandler::BuildOutboundMessage(
    uint16_t run_num, uint16_t loop_count, GbFix &a_fix, float battery_voltage,
    float battery2_voltage, int sail_position, uint8_t diagnostic_message) {
  String log_sentence = "";
  String base_62_date_time;

  switch (this->_message_version) {
  case 2: // long form
    log_sentence += _message_version;
    log_sentence += ",";
    log_sentence += run_num;
    log_sentence += ",";
    log_sentence += loop_count;
    log_sentence += ",";
    log_sentence += String(a_fix.year % 100);
    log_sentence += FormatDateNumber(a_fix.month);
    log_sentence += FormatDateNumber(a_fix.day);
    log_sentence += FormatDateNumber(a_fix.hour);
    log_sentence += FormatDateNumber(a_fix.minute);
    log_sentence += FormatDateNumber(a_fix.second);
    log_sentence += ",";
    log_sentence += String(a_fix.latitude, 4);
    log_sentence += ",";
    log_sentence += String(a_fix.longitude, 4);
    log_sentence += ",";
    log_sentence += String(battery_voltage, 2);
    log_sentence += ",";
    log_sentence += sail_position;
    log_sentence += ",";
    log_sentence += diagnostic_message;
    break;
  case 3: // base62 short form
    log_sentence += _message_version;
    log_sentence += ",";
    log_sentence += ConvertToBase62(run_num);
    log_sentence += ",";
    log_sentence += ConvertToBase62(loop_count);
    log_sentence += ",";
    log_sentence += ConvertToBase62(a_fix.year % 100);
    log_sentence += ConvertToBase62(a_fix.month);
    log_sentence += ConvertToBase62(a_fix.day);
    log_sentence += ConvertToBase62(a_fix.hour);
    log_sentence += ConvertToBase62(a_fix.minute);
    log_sentence += ConvertToBase62(a_fix.second);
    log_sentence += ",";
    log_sentence += ConvertToBase62((long)round((90 + a_fix.latitude) * 10000));
    log_sentence += ",";
    log_sentence +=
        ConvertToBase62((long)round((180 + a_fix.longitude) * 10000));
    log_sentence += ",";
    log_sentence += ConvertToBase62((int)round(battery_voltage * 100));
    log_sentence += ",";
    log_sentence += ConvertToBase62(sail_position);
    log_sentence += ",";
    log_sentence += ConvertToBase62(diagnostic_message);
    break;
  case 4: // long form, 2 batteries
    log_sentence += _message_version;
    log_sentence += ",";
    log_sentence += run_num;
    log_sentence += ",";
    log_sentence += loop_count;
    log_sentence += ",";
    log_sentence += String(a_fix.year % 100);
    log_sentence += FormatDateNumber(a_fix.month);
    log_sentence += FormatDateNumber(a_fix.day);
    log_sentence += FormatDateNumber(a_fix.hour);
    log_sentence += FormatDateNumber(a_fix.minute);
    log_sentence += FormatDateNumber(a_fix.second);
    log_sentence += ",";
    log_sentence += String(a_fix.latitude, 4);
    log_sentence += ",";
    log_sentence += String(a_fix.longitude, 4);
    log_sentence += ",";
    log_sentence += String(battery_voltage, 2);
    log_sentence += ",";
    log_sentence += String(battery2_voltage, 2);
    log_sentence += ",";
    log_sentence += sail_position;
    log_sentence += ",";
    log_sentence += diagnostic_message;
    break;
  }

  Serial.print(F("log_sentence = "));
  Serial.println(log_sentence);
  return log_sentence;
}

String GbMessageHandler::FormatDateNumber(uint8_t number) {
  String tempString;
  if (number < 10) {
    tempString = "0";
  }
  tempString += String(number);
  return tempString;
}

String GbMessageHandler::ConvertToBase62(uint32_t input) {
  // when decoding the base 62 numbers, remember to look for 5 digits. if this
  // is the case, add a "0"!
  const char BASE_62_CHARACTERS[63] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  String base62String;
  while ((input / 62) != 0) {
    base62String = BASE_62_CHARACTERS[(input % 62)] + base62String;
    input = input / 62;
  }
  base62String = BASE_62_CHARACTERS[input] + base62String;
  return base62String;
}
