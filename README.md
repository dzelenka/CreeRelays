## CreeRelays
My prototype:

![screenshot](https://github.com/dzelenka/CreeRelays/blob/master/P1000969.JPG)

This is a fork of the ogiewon/CreeRelays project. I've changed the little Arduino model, but otherwise duplicated his equipment. I wanted to take advantage of his work to build a functional 2x garage door opener. I eventually drifted away from all his code, but we still owe him for the original good work and inspiration!

## Overview
ogiewon wrote a device driver for his hardware device. I used a different paradigm for the project, which felt more "SmartThings" like. I created a very simple button device type to create virtual buttons on my phone, one for each garage door. For the CreeRelay hardware I just used the stock Cree Bulb device type. All the hard work on the SmartThings system is done by a smartapp. 

When the smartapp is installed it askes for: 1. button(s) to get events from, 2. the Cree device, 3. the Relay Number (1-8) and 4. the action to perform (1-4).

The actions I defined are: 1. Off, 2. On, 3. Toggle, and 4. Button Push (on-delay-off).

The smartapp simply listens for button events and sets the Cree device "switchLevel" to a specific level. The specific level is calculated from the relay number and the action to perform. After a 1000ms delay, the Cree device is set to level "99", which does nothing. The Arduino sketch is waiting for changes to the switchLevel to perform actions. By "parking" the Cree device at level "99" we are able to perform the same action (Toggle or Button Push) twice in a row and have each action be a change in the switchLevel. 

The Cree device has some quirks that I had to work around to get dependable performance. One quirk is a level drift of +-5%. To get around that I limited the levels to multiples of 3 and rounded. The other quirk (feature?) of the Cree is that it turns off when the switchLevel is set to "0". It caused some unexplained problems for me, so I just quit using level "0". The resulting set of switchLevels was [3,6,9...96,99]. I reserved level "99" as a parking spot, leaving exactly 32 values for signaling.

Here is how I calculate the switchLevel:
   1. Subtract 1 from the Relay number (1-8) to get a value 0-7.
   2. Subtract 1 from Action number (1-4) to get a value 0-3.
   3. Multiply the action by 8 (or shift left 3 times) and add it to the relay to get a value 0-31.
   4. Add one to the result and multiply by 3 to get a value of 3-96, which is the switchLevel to set!

The Arduino sketch is also quit simple. I simplified the interrupt technique (only because I couldn't figure out what ogiewon was doing with his). The main loop runs every 50ms and does two tasks: react to changes in the switchLevel, and perform timer functions. The timer function is used for the Button Push. It waits 20 loops and then turns off the relay. When the sketch detects a change in the switchLevel it does the binary math in reverse and the performs the resulting action on the resulting relay.

## Code
CreeRelays consists of three parts:
- The CreeRelays Arduino sketch
- A simple virtual button device type
- The CreeRelays smartapp

## Hardware Requirements
CreeRelays requires:
- Arduino Nano, UNO, or similar
- Cree Connected LED Bulb's Zigbee Communication Board
- Relay(s) (I used a 2 relay module)

## Hardware Setup Instructions
Join your Cree Connected Bulb to your hub using your phone's SmartThings App BEFORE disassembling the bulb!!! (To connect it later you need to power cycle it 4 times and then leave it on.)

After removing the Zigbee board from the Cree Connected bulb, attach it to the Arduino as follows:
 - Zigbee Board Pin 1 to Arduino GND
 - Zigbee Board Pin 2 to Arduino 3.3V
 - Zigbee Board Pin 3 to Arduino Pin 2 (PWM)
 - Zigbee Board Pin 4 to Arduino Pin 3 (On/Off) (not used by my code)
 - Relay GND to Arduino GND
 - Relay IN1 to Arduino Pin 4
 - Relay IN2 to Arduino Pin 5
 - Relay VCC to Arduino 5V
  
##CreeRelay to Garage Door Buttons
The original project ogiewon/CreeRelays was a proof of concept. I've simplified the code and tried to make it more SmartThings like. I've got a working prototype and intend to connect it to an Adafruit Metro Mini 328. I'll 3D print a case and install it in my garage. Pictures to follow...

My smartapp allows multiple button devices to activate the relays. I could have a physical fob with a button in the car to replace my existing door opener. 

Update: Here are some snaps of my final result, ready to mount in the garage:
![screenshot](https://github.com/dzelenka/CreeRelays/blob/master/P1000970.JPG)
![screenshot](https://github.com/dzelenka/CreeRelays/blob/master/P1000971.JPG)
