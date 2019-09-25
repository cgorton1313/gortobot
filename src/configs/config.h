/* These are the global configs */

#ifndef RESET_EEPROM
#define RESET_EEPROM false // sets runNum back to 0
#endif

// #define DEBUG // comment this line out to disable serial
#ifdef DEBUG
  #define DEBUG_BEGIN(x) Serial.begin(x)
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINT_XY(x,y) Serial.print(x,y)
#else
  #define DEBUG_BEGIN(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT_XY(x,y)
#endif
