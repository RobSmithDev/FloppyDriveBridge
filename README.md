# FloppyDriveBridge

# FloppyDriveBridges for *UAE
Bridges are what I have chosen to call the name of each 'real' drive added to the emulator.
This folder contains the latest implementations for real floppy disk access within *UAE emulators.
For these to work, the main emulator must have had the ''disk.cpp'' and ''expansion.cpp'' updated from the original
This only needs doing once, and again if it is updated (ie: the no-click support below). 
You can see these files at https://github.com/RobSmithDev/WinUAE

There is also a DLL version of this library for use with the official WinUAE build.

# Licence
The majority is covered by the GNU GPL3 with the exception of:
	floppybridge_lib.cpp
	floppybridge_lib.h
	floppybridge_abstract.h
	floppybridge_config.h
	
	Which are separately licenced as UnLicence - see http://unlicense.org

# Updates:
October 2021: Support for HD disks, turbo speed and a few other things - Requires new disk.cpp from https://github.com/RobSmithDev/WinUAE
              Fixed 64-bit build with the FTDI driver
19th June 2021: Massivly improve write system.  Requires new disk.cpp from https://github.com/RobSmithDev/WinUAE
                Small update for Bridge to support 'no-click' option 
                Fixed some issues with thread scheduling on Linux (Pi)
October 2021:   Added Supercard Pro support, HD support, 'Turbo' mode, a DLL version, and loads of bug fixes

# Supported Devices:
## DrawBridge aka Arduino Reader/Writer at https://amiga.robsmithdev.co.uk
Requires a mod to the circuit to give access to the DISKCHANGE pin.  Requires firmware V1.8

## Greeseweazle at https://github.com/keirf/Greaseweazle
Requires firmware 0.27.  Only some boards support the DISKCHANGE pin.  The rest will be simulated by spinning up the disk several times.

## Supercard Pro at https://www.cbmstuff.com/
Requires firmware xxxx

# Contribute
If you have another device you would like to contribute to this or have a suggestion or improvement let me know/make a pull request.

RobSmithDev
