// communication.ino handles creation of the log sentence,
//  satellite/wifi TX and RX, etc.

// MT message starts with message type, then
//  1 (std)-
//  mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z
//  (end)
//  2 (test)- timePerTack (loggingInterval), z (end)

void blinkMessage(int condition) {
  switch (condition) {
  case 1: // continuous quick flash, stops program here
    while (true) {
      digitalWrite(LED_PIN,
                   (millis() / 100) % 2 == 1 ? HIGH : LOW); // rapid blink
    }
    break;
  case 2: // single quick flash
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW);
    break;
  case 3: // continuous on 2, off half
    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(2000);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
    break;
  }
}

void getSerialMonitorOrders() {
  sail.GetPosition();
  Serial.println(F("Ready for orders:"));
  while (!Serial.available())
    ; // wait for the serial data

  // get the order string, turn it into char[] b/c isbd returns that, turn back
  // into string and split it at the commas
  String sentence = Serial.readString();
  size_t rxBufferSize = sentence.length() + 1;
  char rxBuffer[rxBufferSize];
  sentence.toCharArray(rxBuffer, rxBufferSize);
  String newString = (rxBuffer); // necessary?
  // parseRxBuffer(newString);      // could we do String(rxBuffer) or toString?
}

void getFakeOrders() {
  //  1 (std)-
  //  mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z
  //  (end)
  if (sailMode == 't') {
    String fakeOrderString = "3,";
    fakeOrderString += loggingInterval;
    fakeOrderString += ",z";
  } else {
    String fakeOrderString = "1,0,1,360,1,";
    fakeOrderString += loggingInterval;
    fakeOrderString += ",z";
    // parseRxBuffer(fakeOrderString);
  }
}

byte diagnosticMessage() {
  // 1- too many trim pulses and sail is moving
  // 2- sail not moving after trim pulses
  // 4- fram problem
  // 8- rxMessage not valid
  // add these up to determine which combo is being reported
  return (trimRoutineExceededMax * pow(2, 0) + sailNotMoving * pow(2, 1) +
          rxMessageInvalid * pow(2, 3));
}
