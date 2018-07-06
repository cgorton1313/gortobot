#include "gb_message_handler.h"

bool GbMessageHandler::IsValidInboundMessage(String inboundMessage) {
  if (inboundMessage.indexOf("z") == -1) {
    DEBUG_PRINTLN(F("No z found in inbound message."));
    return false;
  }

  if ((inboundMessage.substring(0, inboundMessage.indexOf(','))).toInt() != 1) {
    DEBUG_PRINTLN(F("Message did not start with 1."));
    return false;
  }

  GbSailingOrders tempOrders = ParseMessage(inboundMessage);
  if (!ValidSailingOrders(tempOrders)) {
    DEBUG_PRINTLN(F("Orders were not valid."));
    return false;
  }

  return true;
}

GbSailingOrders GbMessageHandler::ParseMessage(String inboundMessage) {
  GbSailingOrders newSailingOrders;

  // [message_type],[positionA],[timeA],[positionB],[timeB],[interval]
  // standard message = 1,120,20,60,20,30,z
  uint8_t firstCommaIndex = inboundMessage.indexOf(',');
  uint8_t secondCommaIndex = inboundMessage.indexOf(',', firstCommaIndex + 1);
  uint8_t thirdCommaIndex = inboundMessage.indexOf(',', secondCommaIndex + 1);
  uint8_t fourthCommaIndex = inboundMessage.indexOf(',', thirdCommaIndex + 1);
  uint8_t fifthCommaIndex = inboundMessage.indexOf(',', fourthCommaIndex + 1);
  uint8_t sixthCommaIndex = inboundMessage.indexOf(',', fifthCommaIndex + 1);

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

  // transmission will be converted from minutes to seconds
  newSailingOrders.orderedSailPositionA = secondValue.toInt();
  newSailingOrders.orderedTackTimeA = thirdValue.toInt() * 60;
  newSailingOrders.orderedSailPositionB = fourthValue.toInt();
  newSailingOrders.orderedTackTimeB = fifthValue.toInt() * 60;
  newSailingOrders.loggingInterval = sixthValue.toInt() * 60;

  return newSailingOrders;
}

uint8_t GbMessageHandler::GetDiagnosticMessage(GbTrimResult trimResult,
                                               bool rxMessageInvalid) {
  // 1- trimRoutineExceededMax
  // 2- sailStuck
  // 4- fram problem
  // 8- rxMessage not valid
  // 16- sail battery too low
  // add these up to determine which combo is being reported
  return trimResult.trimRoutineExceededMax | trimResult.sailStuck << 1 | 0 |
         rxMessageInvalid << 3 | trimResult.sailBatteryTooLow << 4;
}

String GbMessageHandler::BuildOutboundMessage(
    uint8_t message_version, uint16_t run_num, uint32_t loop_count,
    GbFix &a_fix, float battery_voltage, float battery2_voltage,
    int sail_position, uint8_t diagnostic_message) {

  String log_sentence = "";

  switch (message_version) {
  case 4: // long form, 2 batteries
    log_sentence += message_version;
    log_sentence +=
        LongFormBase(run_num, loop_count, a_fix, battery_voltage,
                     battery2_voltage, sail_position, diagnostic_message);
    break;
  case 5: // base62 form, 2 batteries
    log_sentence += message_version;
    log_sentence +=
        ShortFormBase(run_num, loop_count, a_fix, battery_voltage,
                      battery2_voltage, sail_position, diagnostic_message);
    break;
  }

  return log_sentence;
}

String GbMessageHandler::BuildOutboundMessage(
    uint8_t message_version, uint16_t run_num, uint32_t loop_count,
    GbFix &a_fix, float battery_voltage, float battery2_voltage,
    int sail_position, uint8_t diagnostic_message, float temperature,
    float humidity) {

  String log_sentence = "";

  switch (message_version) {
  case 6: // long form, plus temp/readHumidity
    log_sentence += message_version;
    log_sentence +=
        LongFormBase(run_num, loop_count, a_fix, battery_voltage,
                     battery2_voltage, sail_position, diagnostic_message);
    log_sentence += ",";
    log_sentence += String(temperature, 1);
    log_sentence += ",";
    log_sentence += String(humidity, 1);
    break;
  case 7: // base62 form, plus temp/readHumidity
    log_sentence += message_version;
    log_sentence +=
        ShortFormBase(run_num, loop_count, a_fix, battery_voltage,
                      battery2_voltage, sail_position, diagnostic_message);
    log_sentence += ",";
    log_sentence += ConvertToBase62((int)round(temperature * 10));
    log_sentence += ",";
    log_sentence += ConvertToBase62((int)round(humidity * 10));
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

String GbMessageHandler::ConvertToBase62(int32_t input) {
  // when decoding the base 62 numbers, remember to look for 5 digits. if this
  // is the case, add a "0"!
  const char BASE_62_CHARACTERS[63] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  String base62String;
  bool isNegative = false;
  if (input < 0) {
    isNegative = true;
    input = input * -1;
  }
  while ((input / 62) != 0) {
    base62String = BASE_62_CHARACTERS[(input % 62)] + base62String;
    input = input / 62;
  }
  base62String = BASE_62_CHARACTERS[input] + base62String;

  if (isNegative) {
    base62String = "-" + base62String;
  }

  return base62String;
}

bool GbMessageHandler::ValidSailingOrders(GbSailingOrders ordersToCheck) {
  // tack times have 12 hour max, logging interval has 1 day max
  bool ordersValid = true;
  if (ordersToCheck.orderedSailPositionA < 0 ||
      ordersToCheck.orderedSailPositionB < 0 ||
      ordersToCheck.orderedSailPositionA > 359 ||
      ordersToCheck.orderedSailPositionB > 359 ||
      ordersToCheck.orderedTackTimeA <= 0 ||
      ordersToCheck.orderedTackTimeB <= 0 ||
      ordersToCheck.orderedTackTimeA > 43200 || // 12 hours
      ordersToCheck.orderedTackTimeB > 43200 || // 12 hours
      ordersToCheck.loggingInterval <= 0 ||
      ordersToCheck.loggingInterval > 86400) { // 24 hours
    ordersValid = false;
  }
  return ordersValid;
}

String GbMessageHandler::LongFormBase(uint16_t run_num, uint32_t loop_count,
                                      GbFix &a_fix, float battery_voltage,
                                      float battery2_voltage, int sail_position,
                                      uint8_t diagnostic_message) {
  String longFormBase;
  longFormBase += ",";
  longFormBase += run_num;
  longFormBase += ",";
  longFormBase += loop_count;
  longFormBase += ",";
  longFormBase += String(a_fix.year % 100);
  longFormBase += FormatDateNumber(a_fix.month);
  longFormBase += FormatDateNumber(a_fix.day);
  longFormBase += FormatDateNumber(a_fix.hour);
  longFormBase += FormatDateNumber(a_fix.minute);
  longFormBase += FormatDateNumber(a_fix.second);
  longFormBase += ",";
  longFormBase += String(a_fix.latitude, 4);
  longFormBase += ",";
  longFormBase += String(a_fix.longitude, 4);
  longFormBase += ",";
  longFormBase += String(battery_voltage, 2);
  longFormBase += ",";
  longFormBase += String(battery2_voltage, 2);
  longFormBase += ",";
  longFormBase += sail_position;
  longFormBase += ",";
  longFormBase += diagnostic_message;
  return longFormBase;
}

String GbMessageHandler::ShortFormBase(uint16_t run_num, uint32_t loop_count,
                                       GbFix &a_fix, float battery_voltage,
                                       float battery2_voltage,
                                       int sail_position,
                                       uint8_t diagnostic_message) {
  String shortFormBase;
  shortFormBase += ",";
  shortFormBase += ConvertToBase62(run_num);
  shortFormBase += ",";
  shortFormBase += ConvertToBase62(loop_count);
  shortFormBase += ",";
  shortFormBase += ConvertToBase62(a_fix.year % 100);
  shortFormBase += ConvertToBase62(a_fix.month);
  shortFormBase += ConvertToBase62(a_fix.day);
  shortFormBase += ConvertToBase62(a_fix.hour);
  shortFormBase += ConvertToBase62(a_fix.minute);
  shortFormBase += ConvertToBase62(a_fix.second);
  shortFormBase += ",";
  shortFormBase += ConvertToBase62((long)round((90 + a_fix.latitude) * 10000));
  shortFormBase += ",";
  shortFormBase +=
      ConvertToBase62((long)round((180 + a_fix.longitude) * 10000));
  shortFormBase += ",";
  shortFormBase += ConvertToBase62((int)round(battery_voltage * 100));
  shortFormBase += ",";
  shortFormBase += ConvertToBase62((int)round(battery2_voltage * 100));
  shortFormBase += ",";
  shortFormBase += ConvertToBase62(sail_position);
  shortFormBase += ",";
  shortFormBase += ConvertToBase62(diagnostic_message);
  return shortFormBase;
}
