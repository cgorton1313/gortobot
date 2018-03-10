#include "gb_sentence_builder.h"

GbSentenceBuilder::GbSentenceBuilder(byte message_version) {
        _message_version = message_version;
}

String GbSentenceBuilder::Sentence(unsigned int run_num, unsigned int loop_count, GbFix &a_fix,
                                   float battery_voltage, float battery2_voltage, int sail_position, byte diagnostic_message) {
        String log_sentence = "";
        String base_62_date_time;

        switch (this->_message_version) {
        case 2: // long form
                log_sentence += _message_version;
                log_sentence += ",";
                log_sentence += run_num;
                log_sentence += ",";
                log_sentence += loop_count;
                log_sentence += ",";
                log_sentence += String(a_fix.year % 100);
                log_sentence += FormatDateNumber(a_fix.month);
                log_sentence += FormatDateNumber(a_fix.day);
                log_sentence += FormatDateNumber(a_fix.hour);
                log_sentence += FormatDateNumber(a_fix.minute);
                log_sentence += FormatDateNumber(a_fix.second);
                log_sentence += ",";
                log_sentence += String(a_fix.latitude, 4);
                log_sentence += ",";
                log_sentence += String(a_fix.longitude, 4);
                log_sentence += ",";
                log_sentence += String(battery_voltage, 2);
                log_sentence += ",";
                log_sentence += sail_position;
                log_sentence += ",";
                log_sentence += diagnostic_message;
                break;
        case 3: // base62 short form
                log_sentence += _message_version;
                log_sentence += ",";
                log_sentence += ConvertToBase62(run_num);
                log_sentence += ",";
                log_sentence += ConvertToBase62(loop_count);
                log_sentence += ",";
                log_sentence += ConvertToBase62(a_fix.year % 100);
                log_sentence += ConvertToBase62(a_fix.month);
                log_sentence += ConvertToBase62(a_fix.day);
                log_sentence += ConvertToBase62(a_fix.hour);
                log_sentence += ConvertToBase62(a_fix.minute);
                log_sentence += ConvertToBase62(a_fix.second);
                log_sentence += ",";
                log_sentence += ConvertToBase62((long)round((90 + a_fix.latitude) * 10000));
                log_sentence += ",";
                log_sentence += ConvertToBase62((long)round((180 + a_fix.longitude) * 10000));
                log_sentence += ",";
                log_sentence += ConvertToBase62((int)round(battery_voltage * 100));
                log_sentence += ",";
                log_sentence += ConvertToBase62(sail_position);
                log_sentence += ",";
                log_sentence += ConvertToBase62(diagnostic_message);
                break;
        case 4: // long form, 2 batteries
                log_sentence += _message_version;
                log_sentence += ",";
                log_sentence += run_num;
                log_sentence += ",";
                log_sentence += loop_count;
                log_sentence += ",";
                log_sentence += String(a_fix.year % 100);
                log_sentence += FormatDateNumber(a_fix.month);
                log_sentence += FormatDateNumber(a_fix.day);
                log_sentence += FormatDateNumber(a_fix.hour);
                log_sentence += FormatDateNumber(a_fix.minute);
                log_sentence += FormatDateNumber(a_fix.second);
                log_sentence += ",";
                log_sentence += String(a_fix.latitude, 4);
                log_sentence += ",";
                log_sentence += String(a_fix.longitude, 4);
                log_sentence += ",";
                log_sentence += String(battery_voltage, 2);
                log_sentence += ",";
                log_sentence += String(battery2_voltage, 2);
                log_sentence += ",";
                log_sentence += sail_position;
                log_sentence += ",";
                log_sentence += diagnostic_message;
                break;
        }

        Serial.print(F("log_sentence = "));
        Serial.println(log_sentence);
        //log_sentence.toCharArray(txBuffer, log_sentence.length() + 1);
        return log_sentence;
}

String GbSentenceBuilder::FormatDateNumber(byte number) {
        String tempString;
        if (number < 10) {
                tempString = "0";
        }
        tempString += String(number);
        return tempString;
}

String GbSentenceBuilder::ConvertToBase62(unsigned long input) {
        // when decoding the base 62 numbers, remember to look for 5 digits. if this is the case, add a "0"!
        const char BASE_62_CHARACTERS[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        String base62String;
        while ((input / 62) != 0) {
                base62String = BASE_62_CHARACTERS[(input % 62)] + base62String;
                input = input / 62;
        }
        base62String = BASE_62_CHARACTERS[input] + base62String;
        return base62String;
}
