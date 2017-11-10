#include <Arduino.h>
#include <navigation/gb_fix.h>

#ifndef gb_sentence_builder_h
#define gb_sentence_builder_h

class GbSentenceBuilder {

private:
    byte _message_version;
    String FormatDateNumber(byte number);

public:
    GbSentenceBuilder(byte message_version);
    String Sentence(unsigned int run_num, unsigned int loop_count, GbFix &a_fix, float battery_voltage);
};

#endif
