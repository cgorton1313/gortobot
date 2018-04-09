# gortobot
Gortobot is a semi-autonomous sailing drone. This is its code repository.

## Overview
The purpose of the project is to control a small, unmanned sailing drone on a long oceanic voyage. So far, two trans-Atlantic attempts have been made (in 2015 and 2016), both launching from Gloucester, Massachusetts, USA. The first attempt lasted more than 11 days and 450 nautical miles. The second attempt had a rough start and contact was lost after 8 days, but the vessel drifted and was recovered 265 days later in the Azores, 1800 nautical miles away!

This repository is for software artifacts. More information on the physical design and past attempts can be found here:
https://sites.google.com/site/gortobot/

## Framework/Platform
Arduino was chosen as the framework on which to develop Gortobot for several reasons:
- Lower learning curve than pure C++
- Abundance of libraries to choose from, especially for the IridiumSBD modem
- Excellent ecosystem of devices, documentation, and online resources

Atmel AVR is the obvious platform. Gortobot #1 and #2 used a 5V Arduino Pro Mini with ATmega328 (16MHz). Gortobot #3 is being developed on an Arduino Mini Mega2560, especially for the four hardware serial ports.

## Tooling
Platformio on the Atom IDE was chosen for the more advanced features (compared to the Arduino IDE) and for unit testing.

## Code Structure
There are no typical `.ino` Arduino sketch files. In order to have more control and a "proper" C++ program, the entry point is main.cpp. Arduino.h is included, though.

The general idea is that the `main.cpp` handles the typical `setup()` and `loop()`. There is no "Gortobot" object since I can think of no instance where a single program would instantiate more than one. Instead, the main.cpp instantiates the objects that the vessel will need:
- satcom
- gps
- wifi (for backyard and rooftop testing)
- batteries
- sail
- message handler
- watch stander

Where possible, wrappers are used so that testing and switching libraries is simpler.

After `setup()` does some housekeeping, the `loop()` continuously performs the following:
- get the vessel's position from the GPS
- build a message from the position and other onboard data points
- transmit the outboard message by wifi and/or satcom
- receive and parse the inbound message
- act on the current orders until it's time to transmit again

## Testing
This area is still a work-in-progress. Platformio Plus (paid) allows for unit testing. However, I haven't figured out how to use a mocking framework on the AVR. Conversely, unit testing can be conducted natively (Windows for me) but then I would need to mock Arduino itself.

This leaves me, for now, with no way to mock dependencies like the battery, gps, etc., when testing each component. Still, there is value in unit testing as many of the public methods as I can. It also seems to be a way to deprecate my practice of using all sorts of configuration switches in the main.cpp to test various scenarios.
