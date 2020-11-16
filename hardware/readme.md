# Hardware to drive a Dekatron

Option 1: Buy a dekatron-driving arduino shield (eBay)

Option 2: Roll your own breadboard with discrete materials to generate the 450V and input signals needed - a simple 12V to 450V HV supply is cheap on eBay and. Honestly not as hard as it sounds - Sprite did it here and the main page readme has several examples of how this can be done with really cheap off the shelf components.

Option 3: Have your own PCB Arduino shield fabricated (what I did)

## What's in here
This folder contains the schematics from which I generated a PCB design which directly mates to an Arduino Uno.
The PCB had several aims, 'most' of which were achieved
  * **Drive 2 dekatrons from a single Arduino** with potential for cross-Dekatron triggering (in software through NDX detection on Arduino)
  * **Drive both two and three guide Dekatrons** (different drive schemes hard in hardware but microcontrollers make this easy nowadays) which requires a variety of extra output pins to be utilised
  * **Drive both Neon and Argon Dekatrons** (only partly successful). This is hard - Neon (red) and Argon (purple/violet) Dekatrons require different operating voltages and have different maximum current draw. This part of the hardware is partly a success - the schematic here works perfectly for having two options for maximum current draw but does not seem to have high enough voltage for all Argon Dekatrons. OG-3 (3-guide, Argon) works fine, but A103 and GS10D do not work well - struggling to strike. It doesn't help that Argon Dekatrons are notoriously fickle.
  
  So part success - this schematic has perfectly driven every Neon 2-guide Dekatron i've tried flawlessly, with NDX detection working fine as well but has only been partly successful at Argon 2 and 3-guide dekatrons.
      * 2-guide dekatrons **works great**
      * 3-guide dekatrons **works great**
      * Neon/red Dekatrons **works great**
      * Argon Dekatrons **dodgy** Some lower voltage ones (OG3) work great. Others don't work at all
      
You may decide you are able to modify this PCB to fix the issues, or simply live with them.
