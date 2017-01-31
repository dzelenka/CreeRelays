## CreeRelays
SmartThings + Cree Connected LED Bulb Zigbee board + Arduino = Cheap Home Automation Potential!

![screenshot](https://github.com/dzelenka/CreeRelays/blob/master/P1000969.JPG)

This is a fork of the ogiewon/CreeRelays project. I've changed the little Arduino model, but otherwise duplicated his equipment. I wanted to take advantage of his work to build a functional 2x garage door opener. I eventually drifted away from all his code, but we still owe him for the original good work and inspiration!

## Overview
ogiewon wrote a device driver for his hardware device. I used a different paradigm for the project, which felt more "SmartThings" like. I created a very simple button device type to create virtual buttons on my phone, one for each garage door. For the CreeRelay hardware I just used the stock Cree Bulb device type. To connect the buttons to the CreeRelay device I used a smartapp. The smartapp simply listens for butten events and sets the Cree bulb "switchLevel" to a specific level. When smartapp is added it askes for button(s) to listen for, the Cree Relay device to set, and the relay number to activate. I multiply the relay number by 10 to determine the switchLevel value. (i.e. Relay 5 is activated by setting switchLevel to 50%.)

The only unusual thing done by the smartapp is to set the switchLevel to 1% after a 1000ms delay. (Turning off the Cree device after a delay caused unpredictable results). Otherwise I could have used a stock smartapp.

The Arduino sketch is also quit simple. I simplified the interrupt technique only because I couldn't figure out what ogiewon was doing with his. It worked fine, but I needed to understand how. All the sketch does is read the Cree switchLevel and turn it into a value between 0 and 10. The relay is active until the switchLevel/value changes.

I found the switchLevel from the Cree board drifted a bit, around +-0.5%. I think this code could be modified to run 100 relays, but much safer to run 50 relays. Using 32 (2^5) values from the Cree board you could run 5 relays, each having and on/off value.

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
