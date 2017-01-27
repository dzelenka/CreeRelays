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
  
##Done for now...  
  
![screenshot](https://cloud.githubusercontent.com/assets/5206084/7218058/35927de6-e625-11e4-8915-0ff51ccc8f30.JPG)
![screenshot](https://cloud.githubusercontent.com/assets/5206084/7218060/3bc030aa-e625-11e4-81d7-e59e2e75df42.JPG)

##CreeRelays Arduino Setup Instructions
- Join your Cree Connected Bulb to your hub using your phone's SmartThings App BEFORE disassembling the bulb!!!
- Download the CreeRelays repository.
- This folder structure should mirror that of your local Arduino directory. 
  - On Mac, it's located in `~/Documents/Arduino/`.
  - On Windows, it's located in `C:\My Documents\Arduino`.
- Look inside the `Arduino/Sketches` folder of the repo.
- Copy and paste the `CreeRelays` sketch folder into your local Arduino sketches directory. If you haven't created any sketches, you may not see the folder. In this case, feel free to create it.


##CreeRelays SmartThings Device Type Installation Instructions
- Join your Cree Connected Bulb to your hub using your phone's SmartThings App BEFORE disassembling the bulb!!!
- Create an account and/or log into the SmartThings Developers Web IDE.
- Click on My Device Types from the navigation menu.
- Click on  + New SmartDevice button.
- Fill in the Name field as CreeRelays and click on the Create button. Don't worry about filling out everything else.
- Paste the code from the CreeRelays.device.groovy file from this repo.
- Click on  Save  in the IDE.
- Click on  Publish -> For Me  in the IDE.
- Click on My Devices from navigation menu
- Select your "Arduino ThingShield" device from the list
- Click the Edit button at the bottom of the screen
- Change the Type to "CreeRelays"
- Click the Update button at the bottom of the screen
- On your phone, log out of SmartThings in the app, and then log back into SmartThings to refresh its settings
- Your CreeRelays Device Tile should now look like the image above in this ReadMe

##CreeRelays Usage
- This is essentially a proof of concept example.  You can control up to 8 relays with this example as-is.  My intention was to share this project as an inspiration for others to kick-start any other efforts that desire a very low cost way to control real-world devices from SmartThings.
- Pressing any of the "Relay 1" through "Relay 8" tiles on your phone will "turn on the bulb and set the dim level to 1 through 8 respectively.
- Based on the dim level selected, the arduino sketch will turn on a corresponding digital output (which could be conncted to a relay.)
- Pressing a different "Relay n" tile will turn off the previous output, and turn on the new output
- Pressing the main "Lightbulb" tile will either turn on or turn off the output which corresponds to the current Dim Level
- Only Dim Levels 1 through 8 are currently being handled by the Arduino sketch.  Using the slider control to set dim levels above 8 or below 1 will turn off all of the output pins.  Note: All 100 dim levels are properly recognized in the Arduino Sketch, but I only have code in place for values 1 though 8 in this example.
- Only one digital output can be on at a time in the current example.  For more complicated designs, the exercise is left up to the student!

##CreeGarageDoorButtons

- This is an attempt to utilize this proof of concept into a reliable garage door opener with two buttons. I have created a working copy of the parent project with an Arduino Uno, a Cree Zigbee radio, and a dual relay.

- a picture should go here.

- I have a Adafruit Metro Mini 328 in the mail for my production version.

- I've simplified the parent code down to two relays. The big job left will be to convert the switch funtion to buttons...


