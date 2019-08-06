#include <stdint.h>

#ifndef GB_CONSTS_H
#define GB_CONSTS_H

static const uint8_t DELAY_FOR_SERIAL =
    10; // ms to delay so serial ouput is clean
static const uint32_t CONSOLE_BAUD = 9600, WIFI_BAUD = 115200,
                      GPS_BAUD = 38400, SAT_BAUD = 19200;
static const uint16_t SAT_CHARGE_TIME =
    10; // seconds to wait at start-up for super-capacitor
static const uint16_t ISBD_TIMEOUT =
    300; // seconds to try getting isbd success each time, 5 min default
static const uint8_t WIFI_ATTEMPT_LIMIT =
    3; // number of times to try connecting to wifi
static const float MINIMUM_BATTERY_VOLTAGE =
    3.5; // system will wait for charging at this low voltage threshold
static const float BATTERY_OKAY_VOLTAGE =
    3.6; // system will resume program at this voltage threshold
static const uint16_t BATTERY_WAIT_TIME =
    60; // seconds to wait between checking for batteryOkay

// Message versions (evolution):
//    2 = long form
//    3 = base62
//    4 = 2 + two batteries
//    5 = 3 + two batteries
//    6 = 4 + temp/humidity
//    7 = 5 + temp/humidity
static const uint8_t MESSAGE_VERSION = 7;

static const int16_t MAST_POSITION_CALIBRATION = 0; // Use this if the mast is mounted slightly askew.
static const uint16_t MIN_SAIL_ANGLE = 0,
                      MAX_SAIL_ANGLE = 359; // limits for sail
static const uint16_t TRIM_ROUTINE_MAXIMUM_SECONDS =
    120; // max number of trim seconds allowed to get to ordered position

#endif
