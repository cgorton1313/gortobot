#include <stdint.h>
static const uint8_t DELAY_FOR_SERIAL =
    10; // ms to delay so serial ouput is clean
static const uint32_t CONSOLE_BAUD = 115200, WIFI_BAUD = 115200,
                      GPS_BAUD = 38400;
static const uint32_t SAT_BAUD = 19200;
static const uint16_t SAT_CHARGE_TIME =
    30; // seconds to wait at start-up for super-capacitor
static const uint16_t ISBD_TIMEOUT = 600; // seconds to try getting isbd success
static const uint16_t FAILURE_RETRY = 600; // seconds to wait after tx failure
static const uint8_t WIFI_ATTEMPT_LIMIT =
    3; // number of times to try connecting to wifi
static const float MINIMUM_BATTERY_VOLTAGE =
    3.4; // system will wait for charging at this low voltage threshold
static const float BATTERY_OKAY_VOLTAGE =
    3.5; // system will resume program at this voltage threshold
static const uint16_t BATTERY_WAIT_TIME =
    2; // seconds to wait between checking for batteryOkay
static const uint8_t MESSAGE_VERSION =
    4; // 2 = long form, 3 = base62, 4 = base62 and 2 batteries
static const uint16_t MIN_SAIL_ANGLE = 0,
                      MAX_SAIL_ANGLE = 360; // limits for sail
static const uint16_t TRIM_ROUTINE_MAXIMUM_SECONDS =
    900; // max number of trim seconds allowed to get to ordered position.
         // testing shows 450 should be max
