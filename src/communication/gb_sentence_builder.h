#include <Arduino.h>
#include <navigation/gb_fix.h>

class GbSentenceBuilder {

private:
    byte _message_version;

public:
    GbSentenceBuilder(byte message_version);
    String Sentence(unsigned int run_num, unsigned int loop_count, GbFix &a_fix);
};
