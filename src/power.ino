// power.ino handles solar, battery, power reporting, sleeping


static void gortoNap(int seconds) {
        for (int i = 0; i < seconds; i++) {
                sleep.sleepDelay(1000);
                delay(DELAY_FOR_SERIAL);
        }
}
