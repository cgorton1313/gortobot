// navigation.ino gets Gortobot's position, either real or fake

#include <Arduino.h>

static void getFix(char fixType) {
        switch (fixType) {
        case 'r':
                processVoltageData(); // make sure there's enough juice to do this
                delay(2005); // to trigger staleFix because millis stop during sleep
                gpsOn();
                while (!fixAcquired || staleFix) { // should we time-out here?
                        processGPSData();
                }
                gpsOff();
                break;
        case 'f':
                fakeGPS();
                break;
        }
}

static void gpsOn() {
        gpsSS.begin(gpsBaud);
        digitalWrite(gpsPowerPin, HIGH);
        Serial.println(F("GPS on."));
        staleFix = true;
        fixAcquired = false;
}

static void gpsOff() {
        gpsSS.end();
        delay(delayForSerial);
        digitalWrite(gpsPowerPin, LOW);
        Serial.println(F("GPS off."));
}

static void processGPSData() {
        while (gpsSS.available() > 0) {
                tinyGPS.encode(gpsSS.read());
        }
        latitude = tinyGPS.location.lat();
        longitude = tinyGPS.location.lng();
        year = tinyGPS.date.year();
        month = tinyGPS.date.month();
        day = tinyGPS.date.day();
        ddmmyy = tinyGPS.date.value(); // do we need this? check logData
        hour = tinyGPS.time.hour();
        minute = tinyGPS.time.minute();
        second = tinyGPS.time.second();
        hhmmss = tinyGPS.time.value() / 100; // gets rid of centiseconds, do we need this? check logData
        satellites = tinyGPS.satellites.value();
        fixAcquired = tinyGPS.location.isValid() && tinyGPS.date.isValid() && tinyGPS.time.isValid();
        staleFix = fixAcquired && (tinyGPS.location.age() > 2000 || tinyGPS.date.age() > 2000);
}

static void fakeGPS() { // could remove random()'s and do without math.h if necessary
        latitude = random(-800000, 800000) / 10000.0;
        longitude = random(-1800000, 1800000) / 10000.0;
        year = 2016 + random(5); // any year b/w 2016 and 2020
        month = 1 + random(12);
        day = 1 + random(31); // will sometimes give bogus dates
        hour = 1 + random(23);
        minute = 1 + random(59);
        second = 1 + random(59);
        satellites = 1;
        fixAcquired = true;
        staleFix = false;
}
