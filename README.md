# dekatron-speed

A project using both hardware and software to fetch current broadband usage from a router with SNMP (inOctets and outOctets) and display this as a percentage speed on a variable speed Dekatron counting tube, controlled by ESP8266 or similar.

![Gif of Dekatron Speed](https://github.com/elegantalchemist/dekatron-speed/blob/main/images/dekatron%20speed%20gif.gif?raw=true)

## Materials
* **Dekatron counting tube** - recommended basic double pulse tubes like the OG4 or A101. Easily available on eBay about £10 or so plus shipping.
* **ESP8266** - primary controller for the code and signalling the Dekatron - I used a NodeMCU/ESP8266 board easily available on Amazon for about £4 each.
* **Dekatron power/interface** - the most awkward part, Dekatrons need to be driven at about 400-450V and while the drive process is simple - the high voltage introduces complexity. Although the voltage is high, maximum current is only about 500microamps so not in danger levels - you can use voltage boosters to drive 5V all the way up to 450V. These can simply be bought from eBay as kits (search arduino dekatron), or you can purchase or make a high voltage power supply and then use this to add interfaces to microcontrollers with some basic resistors/transistors yourself easily - some helpful links below and there is also a folder in this project containing a schematic for a board of my own design being used here;
  * https://www.smbaker.com/interfacing-dekatron-tubes-to-a-microcontroller
  * https://threeneurons.files.wordpress.com/2011/08/dek_tutor_b.jpg
  * https://threeneurons.wordpress.com/dekatron-stuff/
* **A case** - I recommend starting with some cardboard and going from there! The source files here include the case I used here but a word of warning - it's far from ideal, the bent acrylic is very hard to do and I failed multiple times to bend it.

## Building this Hardware
First of all you should be vaguely familiar with Dekatron driving. I recommend some time researching, especially threeneurons excellent site https://threeneurons.wordpress.com/dekatron-stuff/ and understanding the drive mechanism. With modern microcontrollers its fairly simple.

Second you need to build out these components. ESP8266 takes USB power in, VIN provides 5v to the Arduino board used here and the ESP8266 digital pins D3, D5 and D2 interface with the Dekatron control board.

A standard double pulse Dekaton control runs Anode (A), Guide Cathode (K0 NDX), Other Cathodes (K1-9), Guide1 and Guide2 to various pins which differ depending on which model of Dekatron you use. This site contains excellent datasheet resources on pin outs for Dekatrons - this link is for the OG4 tube http://www.tube-tester.com/sites/nixie/datdekat/OG4/og4.htm

The Dekatron should be connected to your home made or bought Dekatron control system, this to the ESP8266 and you're ready to start sending signals.

## Building this software
I have used the Arduino IDE for the entire project, this can control ESP8266 boards with pretty much the same code as Arduino boards if you download and install the ESP8266 board manager
* ESP8266 board manager for Arduino https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

The software has a few small dependencies which are listed at the top of the code and also here to download and add as Arduino libraries:
* shortbloke's Arduino SNMP manager https://github.com/shortbloke/Arduino_SNMP_Manager
* millisdelay https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html#using

## Using this Library
This library contains several variables which you need to adjust
* **Your wifi info** - self explanatory I hope
* **SNMP Device Info** - the device on your network you're going to ping for SNMP info. You need a device which has and allows SNMP access and you need to set the IP address and community string here. Some devices require you to use a read only community so be careful about using rocommunity/rwcommunity types. I found the best piece of software to interrogate an SNMP agent simply is Paessler SNMP Tester 3.0 available here : https://www.paessler.com/tools/snmptester - this is a lightweight tool which allows you to ping a list of interfaces and check your requests are going through. It is NOT a full OID walker so you can't pull OIDs here if your SNMP agent doesn't use the ones specified here which are the standard ones.
* **OID inOCtets/outOctets** - you need to alter the OIDs to have the correct interface number at the end here, best way to do this is get the Paessler SNMP tester to 'scan available standard interfaces' which should hopefully give you a self explanatory list of interface numbers. For example, i'm using .22 on a UDM-Pro.
* **Dekatron Settings** - you may need to change pin outs here. On the ESP8266 pins are 'Dx' and on a standard Arduino they are just 'x' - this is really important to remember if you're using Arduino for testing the hardware and switching to ESP8266 for the actual project. Arduino is much faster for testing general control!
* **Down Speed/Up Speed** - these should be set to your expected maximum line speed, the units are 'bps' for bits per second.

## Testing
I recommend testing your hardware Dekatron setup with some very basic sketches before progressing to the full sketch here. There are some examples in the examples folder which can be used to test that your Dekatron hardware interface is working OK. If you have full control of your Dekatron and you've confirmed your SNMP agent is responding, now's time to upload the full sketch and test it out! 

## Feedback
I'd love to see feedback and projects which have used this library.

## Credits
 Many thanks to shortbloke who wrote an excellent SNMP library which he used to output to a display and was modified for this project to output to Dekatron
 https://github.com/shortbloke/Broadband_Usage_Display
  
 Many thanks to Sprite of sprites mods who provided the inspiration to do this and useful code on how
 to drive Dekatrons from an Arduino/ESP8266
 https://spritesmods.com/?art=dekatron
 
 Many thanks to threeneurons who has an amazing site covering some really in depth stuff on how Dekatrons work, how they get driven and a huge number of schematics covering various situations and types of Dekatron.
 https://threeneurons.wordpress.com/dekatron-stuff/
