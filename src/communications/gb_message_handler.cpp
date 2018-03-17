#include "gb_message_handler.h"

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
      newSailingOrders.orderedSailPositionA =
          secondValue.toInt(); // tolerates spaces
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
    case 2: {                          // sail test mode 2,10,z is 10 min test
      newSailingOrders.sailMode = 't'; // test sail mode
      newSailingOrders.orderedSailPositionA = 180;
      newSailingOrders.orderedTackTimeA = 1;
      newSailingOrders.orderedSailPositionB = 180;
      newSailingOrders.orderedTackTimeB = 1;
      byte firstCommaIndex = inboundMessage.indexOf(',');
      byte secondCommaIndex = inboundMessage.indexOf(',', firstCommaIndex + 1);
      String secondValue =
          inboundMessage.substring(firstCommaIndex + 1, secondCommaIndex);
      unsigned long tempLoggingInterval = secondValue.toInt();
      if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {
        newSailingOrders.loggingInterval = tempLoggingInterval;
      }
      break;
    }
    // TODO: implement this: directly setting ordered sail position not yet
    // supported
    // case 3: { // not to be used in real sailing
    //   sailMode = 's';
    //   byte firstCommaIndex = inboundMessage.indexOf(',');
    //   byte secondCommaIndex =
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

byte GbMessageHandler::GetDiagnosticMessage() {
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
