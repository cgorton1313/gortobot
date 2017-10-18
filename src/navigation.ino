// navigation.ino gets Gortobot's position, either real or fake


static void getFix(char fixType) {
        switch (fixType) {
        case 'r':
                //processVoltageData(); // make sure there's enough juice to do this
                delay(2005); // to trigger staleFix because millis stop during sleep
                gpsOn();
                fixDone = false;
            	while (!fixDone)
            	{
            		while (gps.available(gpsPort))
            		{
            			fix = gps.read();
            			if (fix.valid.location && fix.valid.date && fix.valid.time && (fix.satellites > 3) ) {
            				Serial.print("Fix: ");
            				Serial.print(fix.latitude(), 4);
                            latitude = fix.latitude();
            				Serial.print(", ");
            				Serial.print(fix.longitude(), 4);
                            longitude = fix.longitude();
            				Serial.print(", ");
            				NeoGPS::time_t & dt = fix.dateTime;
            				Serial << dt;
                            year = fix.dateTime.year;
                            month = fix.dateTime.month;
                            day = fix.dateTime.date;
                            hour = fix.dateTime.hours;
                            minute = fix.dateTime.minutes;
                            second = fix.dateTime.seconds;
            				Serial.print(", ");
            				Serial.print(fix.satellites);
            				Serial.println();
            				fixDone = true;
            			}
            		}
            	}
                gpsOff();
                break;
        case 'f':
                fakeGPS();
                break;
        }
}

static void gpsOn() {
        gpsPort.begin(gpsBaud);
        digitalWrite(gpsPowerPin, HIGH);
        Serial.println(F("GPS on."));
}

static void gpsOff() {
        gpsPort.end();
        delay(delayForSerial);
        digitalWrite(gpsPowerPin, LOW);
        Serial.println(F("GPS off."));
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
