// sail.ino handles the sails (trimming, reporting, etc.)

// static void trimSail(int orderedSailPosition) {
//   unsigned long trimTimer;
//   int trimSeconds = 0;
//   int totalTrimSeconds = 0;
//   boolean sailIsTrimming = true;
//   // TODO: check battery 2?
//   // processVoltageData();
//   // battery2.Okay();
//   sailPosition = sail.GetPosition();
//   int tempSailPosition = sailPosition;
//   trimRoutineExceededMax = false;
//   sailNotMoving = false;
//
//   if ((abs(sailPosition - orderedSailPosition)) > 4) { // sail is out of trim
//     trimTimer = millis();                              // capture the time now
//     while (((sailPosition - orderedSailPosition) != 0) && sailIsTrimming) {
//       // trim sail
//       digitalWrite(MOTOR_IN_1_PIN, sailPosition > orderedSailPosition);
//       digitalWrite(MOTOR_IN_2_PIN, sailPosition < orderedSailPosition);
//       if ((unsigned long)(millis() - trimTimer) >=
//           1000) {             // 1 second has passed
//         trimTimer = millis(); // reset second timer
//         totalTrimSeconds = totalTrimSeconds + 1;
//         if (totalTrimSeconds > TRIM_ROUTINE_MAXIMUM_SECONDS) {
//           sailIsTrimming = false;
//           trimRoutineExceededMax = true;
//           Serial.println(F("trimRoutineExceededMax"));
//         }
//         sailPosition = sail.GetPosition();
//         if (sailPosition == tempSailPosition) {
//           // sail hasn't moved in the last second
//           trimSeconds = trimSeconds + 1;
//           if (trimSeconds > 10) {
//             // sail hasn't moved in 10 seconds, bad!
//             sailIsTrimming = false;
//             sailNotMoving = true;
//             Serial.println(F("Sail not moving"));
//           }
//         } else { // sail has moved
//           tempSailPosition = sailPosition;
//           trimSeconds = 0;
//           sailNotMoving = false;
//         }
//       }
//     }
//     stopMotors(); // sail is either in position or stuck
//   }
//   Serial.print(F("totalTrimSeconds = "));
//   Serial.println(totalTrimSeconds);
// }

static void stopMotors() {
  digitalWrite(MOTOR_IN_1_PIN, LOW);
  digitalWrite(MOTOR_IN_2_PIN, LOW);
}
