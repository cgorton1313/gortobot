#include <Arduino.h>
#include "gb_fix.h"

#ifndef GB_SENTENCE_BUILDER_H
#define GB_SENTENCE_BUILDER_H

class GbSentenceBuilder {

private:
    uint8_t _message_version;
    String FormatDateNumber(uint8_t number);
    String ConvertToBase62(uint32_t input);

public:
    GbSentenceBuilder(uint8_t message_version);
    String Sentence(uint16_t run_num, uint16_t loop_count, GbFix &a_fix,
        float battery_voltage, float battery2_voltage, int sail_position, uint8_t diagnostic_message);
};

#endif
