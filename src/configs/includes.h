#include "../communications/gb_blinker.h"
#include "../communications/gb_message_handler.h"
#include "../communications/gb_sailing_orders.h"
#include "../communications/gb_satcom.h"
#include "../communications/gb_wifi.h"
#include "../configs/config.h"
#include "../configs/pins.h"
#include "../navigation/gb_fix.h"
#include "../navigation/gb_gps.h"
#include "../power/gb_abstract_battery.h"
#include "../power/gb_real_battery.h"
#include "../sailing/gb_sail.h"
#include "../utilities/gb_air_sensor.h"
#include "../utilities/gb_air_stats.h"
#include "../utilities/gb_utility.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>
