#include "gb_message_handler.h"

GbSailingOrders GbMessageHandler::ParseMessage(String inboundMessage,
                                               GbSailingOrders existingOrders) {
  GbSailingOrders newSailingOrders;
  bool inboundMessageValid = false;

  if (inboundMessage.indexOf("z") >= 0) { // we should parse it
    switch ((inboundMessage.substring(0, inboundMessage.indexOf(',')))
                .toInt()) { // get first value
    case 1: {               // standard message = 1,120,20,60,20,30,z
      // TODO better way of finding commas:
      // https://stackoverflow.com/questions/1894886/parsing-a-comma-delimited-stdstring
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
      inboundMessageValid = CheckSailingOrders(newSailingOrders);
      break;
    }
    default: {
      Serial.println("Not a valid inbound message type.");
      break;
    }
    }
  } else {
    // How to return this?
    inboundMessageValid = false;
    Serial.println(F("Not a valid message (no 'z' found)"));
  }

  if (inboundMessageValid) {
    return newSailingOrders;
  } else {
    return existingOrders;
  }
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
  return 0;
}

// TODO: use
// https://stackoverflow.com/questions/2462951/c-equivalent-of-stringbuffer-stringbuilder
String GbMessageHandler::BuildOutboundMessage(
    uint8_t message_version, uint16_t run_num, uint16_t loop_count,
    GbFix &a_fix, float battery_voltage, float battery2_voltage,
    int sail_position, uint8_t diagnostic_message) {

  String log_sentence = "";

  switch (message_version) {
  case 4: // long form, 2 batteries
    log_sentence += message_version;
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
  case 5: // base62  form, 2 batteries
    log_sentence += message_version;
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
    log_sentence += ConvertToBase62((int)round(battery2_voltage * 100));
    log_sentence += ",";
    log_sentence += ConvertToBase62(sail_position);
    log_sentence += ",";
    log_sentence += ConvertToBase62(diagnostic_message);
    break;
  }

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

bool GbMessageHandler::CheckSailingOrders(GbSailingOrders ordersToCheck) {
  // tack times have 12 hour max, logging interval has 1 day max
  bool ordersValid = true;
  if (ordersToCheck.orderedSailPositionA < 0 ||
      ordersToCheck.orderedSailPositionB < 0 ||
      ordersToCheck.orderedSailPositionA > 359 ||
      ordersToCheck.orderedSailPositionB > 359 ||
      ordersToCheck.orderedTackTimeA < 0 ||
      ordersToCheck.orderedTackTimeB < 0 ||
      ordersToCheck.orderedTackTimeA > 43200 ||
      ordersToCheck.orderedTackTimeB > 43200 ||
      ordersToCheck.loggingInterval < 0 ||
      ordersToCheck.loggingInterval > 86400) {
    ordersValid = false;
  }
  return ordersValid;
}
