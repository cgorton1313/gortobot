#include "gb_message_parser.h"

// Private constructor for static class
GbMessageParser::GbMessageParser() {}

GbSailingOrders GbMessageParser::ParseMessage(String inboundMessage) {
  GbSailingOrders newSailingOrders;
  bool rxMessageInvalid = false;

  if (inboundMessage.indexOf("z") >= 0) { // we should parse it
    switch ((inboundMessage.substring(0, inboundMessage.indexOf(',')))
                .toInt()) { // get first value
    case 1: {               // standard message = 1,120,20,60,20,30,z
      newSailingOrders.sailMode = 'r';       // real sail mode
      // find the commas, probably should move to char []?
      byte firstCommaIndex = inboundMessage.indexOf(',');
      byte secondCommaIndex = inboundMessage.indexOf(',', firstCommaIndex + 1);
      byte thirdCommaIndex = inboundMessage.indexOf(',', secondCommaIndex + 1);
      byte fourthCommaIndex = inboundMessage.indexOf(',', thirdCommaIndex + 1);
      byte fifthCommaIndex = inboundMessage.indexOf(',', fourthCommaIndex + 1);
      byte sixthCommaIndex = inboundMessage.indexOf(',', fifthCommaIndex + 1);
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
      newSailingOrders.orderedSailPositionA = secondValue.toInt(); // tolerates spaces
      newSailingOrders.orderedTackTimeA = thirdValue.toInt();
      newSailingOrders.orderedSailPositionB = fourthValue.toInt();
      newSailingOrders.orderedTackTimeB = fifthValue.toInt();
      unsigned long tempLoggingInterval = sixthValue.toInt();
      if (tempLoggingInterval > 0 &&
          tempLoggingInterval <= 86400) { // 86,400 secs = 1 day
        newSailingOrders.loggingInterval = tempLoggingInterval;
      }
      break;
    }
    case 2: {         // sail test mode 2,10,z is 10 min test
      newSailingOrders.sailMode = 't'; // test sail mode
      newSailingOrders.orderedSailPositionA = 180;
      newSailingOrders.orderedTackTimeA = 1;
      newSailingOrders.orderedSailPositionB = 180;
      newSailingOrders.orderedTackTimeB = 1;
      byte firstCommaIndex = inboundMessage.indexOf(',');
      byte secondCommaIndex =
          inboundMessage.indexOf(',', firstCommaIndex + 1);
      String secondValue =
          inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
      unsigned long tempLoggingInterval = secondValue.toInt();
      if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {
        newSailingOrders.loggingInterval = tempLoggingInterval;
      }
      break;
    }
    // TODO: implement this: directly setting ordered sail position not yet supported
    // case 3: { // not to be used in real sailing
    //   sailMode = 's';
    //   byte firstCommaIndex = inboundMessage.indexOf(',');
    //   byte secondCommaIndex =
    //       inboundMessage.indexOf(',', firstCommaIndex + 1);
    //   String secondValue =
    //       inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
    //   //orderedSailPosition = secondValue.toInt();
    //   newSailingOrders.loggingInterval = 1; // will wait 1 second before asking again
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

  return newSailingOrders;
}
