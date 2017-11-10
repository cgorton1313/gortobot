#include <Arduino.h>
#include <communication/gb_sentence_builder.h>

GbSentenceBuilder::GbSentenceBuilder(byte message_version) {
        _message_version = message_version;
}

String GbSentenceBuilder::Sentence(unsigned int run_num, unsigned int loop_count, GbFix &a_fix, float battery_voltage) {
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
                //getSailPosition();
                //log_sentence += sailPosition;
                log_sentence += ",";
                //log_sentence += diagnosticMessage();
                break;
        case 3: // base62 short form
                /*log_sentence += MESSAGE_VERSION;
                   log_sentence += ",";
                   log_sentence += convertToBase62(runNum);
                   log_sentence += ",";
                   log_sentence += convertToBase62(loopCount);
                   log_sentence += ",";
                   log_sentence += convertToBase62(a_fix.year % 100);
                   log_sentence += convertToBase62(a_fix.month);
                   log_sentence += convertToBase62(a_fix.day);
                   log_sentence += convertToBase62(a_fix.hour);
                   log_sentence += convertToBase62(a_fix.minute);
                   log_sentence += convertToBase62(a_fix.second);
                   log_sentence += ",";
                   log_sentence += convertToBase62((long)round((90 + a_fix.latitude) * 10000));
                   log_sentence += ",";
                   log_sentence += convertToBase62((long)round((180 + a_fix.longitude) * 10000));
                   log_sentence += ",";
                   log_sentence += convertToBase62((int)round(batteryVoltage * 100));
                   log_sentence += ",";
                   getSailPosition();
                   log_sentence += convertToBase62(sailPosition);
                   log_sentence += ",";
                   log_sentence += convertToBase62(diagnosticMessage());*/
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
