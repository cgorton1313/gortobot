#include <Arduino.h>
#include <communication/gb_sentence_builder.h>

GbSentenceBuilder::GbSentenceBuilder(byte message_version) {
    _message_version = message_version;
}

String Sentence(unsigned int run_num, unsigned int loop_count, GbFix &a_fix) {
    return "dummy";
}
