# FloppyDriveBridge

# FloppyDriveBridges for *UAE
Bridges are what I have chosen to call the name of each 'real' drive added to the emulator.
This folder contains the latest implementations for real floppy disk access within *UAE emulators.
For these to work, the main emulator must have had the ''disk.cpp'' and ''expansion.cpp'' updated from the original
This only needs doing once, and again if it is updated (ie: the no-click support below). 
You can see these files at https://github.com/RobSmithDev/WinUAE

# Updates:
19th June 2021: Massivly improve write system.  Requires new disk.cpp from https://github.com/RobSmithDev/WinUAE
Small update for Bridge to support 'no-click' option 
Fixed some issues with thread scheduling on Linux (Pi)

# Supported Devices:
## Arduino Reader/Writer at https://amiga.robsmithdev.co.uk
Requires a mod to the circuit to give access to the DISKCHANGE pin.  Requires firmware V1.8

## Greeseweazle at https://github.com/keirf/Greaseweazle
Requires firmware 0.27.  Only some boards support the DISKCHANGE pin.  The rest will be simulated bu spinning up the disk several times.

# Contribute
If you have another device you would like to contribute to this or have a suggestion or improvement let me know/make a pull request.

RobSmithDev
