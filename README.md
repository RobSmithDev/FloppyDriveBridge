# FloppyDriveBridge

# FloppyDriveBridges for *UAE
Bridges are what I have chosen to call the name of each 'real' drive added to the emulator.
This folder contains the latest implementations for real floppy disk access within *UAE emulators.
For these to work, the main emulator must have had the ''disk.cpp'' and ''expansion.cpp'' updated from the original
This only needs doing once, and again if it is updated (ie: the no-click support below). 
You can see these files at https://github.com/RobSmithDev/WinUAE

There is also a DLL version of this library for use with the official WinUAE build.
Latest version includes several improvements and testing with the help of Dimitris Panokostas aka MiDWaN (Amiberry)

# Licence
The majority of the source code is available multi-licensed under the terms of the Mozilla Public License Version 2.0
as published by Mozilla Corporation and the GNU General Public License, version 2 or later, as published by the Free
Software Foundation, with the exception of:
	floppybridge_lib.cpp
	floppybridge_lib.h
	floppybridge_abstract.h
	floppybridge_config.h
Which are separately licenced as UnLicence - see http://unlicense.org

# Updates:
* April 2024 (1.6)  Sped up the main thread by refactoring and changing how a lock worked that was fixed in 1.5.  It now loads disks *slightly* faster
                    Added support for "Direct Mode" which allows direct MFM buffer reading and writing so you can use this plugin outside of WinUAE 
				    Added some extra commands to the library to make it easier to use outside of WinUAE
					Changed donate link from PayPal to KoFi
					Updated the writing code for DrawBridge, Greaseweazle and SupercardPRO so it now calculates precompensation correctly when writing to disks
					Some refactoring (slightly breaking - namespace changes) to help use this as a shared dynamic library with less header files needed
* Feb 2024 (1.5) Fixed issue with locks on new Apple M1/2 devices for Amiberry
* April 2023 (1.4 still) Fixed issues with serial access on MacOS
* March 2023 (1.4): Fixed issues with stalling mode not working properly, or not as expected
                    Updated stalling mode to work better with diskspare device which seems to be very impatient!
					Updated the icons for the supported hardware and rendered them transparently
* June 2022 (1.3): Fixed Greaseweazle Drive A/B not selectable properly
				   Fixed issue with warning message about diskchange on Greaseweazle always re-appearing
				   Fixed Greaseweazle Shugart support. (**Does not support disk change** - it will manually check for disks. A PC Drive is strongly recommended)
				   Re-worked the profile listing dialog a little 
* June 2022 (1.2): Added support for track 82 and 83 reading
			 Changed the behaviour to disk check to be no-click for manual detection
			 Updated Greaseweazle driver to also support Shugart interfaces
			 Some cosmetic fixes
* End of Feb 2022: Greatly increased compatability due to changes in the PLL and Rotation Extractor handling.  Check the games that didnt boot before!
* February 2022: Fixed random crash in Rotation Extractor
			   Further stability improvements for Linux (or rather not-Windows)
* Janurary 2022: Added support for Supercard Pro (V1.3 firmware required)
               Changed read mode to use a more accurate PLL (taken from SCP design from SCP.cpp in *UAE) which **improves compatability with some games**
			   Added support for DrawBridge new PLL read command for better accuracy (Firmware 1.9.23) - HIGHLY recommended
			   Fixed a few bugs in rotation extractore that caused a few crashes
			   In "Normal" mode, if a perfect revolution cannot be guessed it automatically switches to compatiable for that track
			   Fixed 64-Bit support for Greaseweazle under some linux distros
* October 2021: Support for HD disks, turbo speed and a few other things - Requires new disk.cpp from https://github.com/RobSmithDev/WinUAE
              Fixed 64-bit build with the FTDI driver
* 19th June 2021: Massivly improve write system.  Requires new disk.cpp from https://github.com/RobSmithDev/WinUAE
                Small update for Bridge to support 'no-click' option 
                Fixed some issues with thread scheduling on Linux (Pi)
* October 2021:   Added HD support, 'Turbo' mode, a DLL version, and loads of bug fixes

# Supported Devices:
## DrawBridge aka Arduino Reader/Writer at https://amiga.robsmithdev.co.uk
Requires a mod to the original (pre 2021) circuit to give access to the DISKCHANGE pin.  Requires firmware V1.8+

## Greeseweazle at https://github.com/keirf/Greaseweazle
Requires firmware 0.27 or newer.  Only some boards support the DISKCHANGE pin.  The rest will be simulated by spinning up the disk several times.
Greaseweazle supports Amiga drives, but this isn't recommended as the DISKCHANGE pin isn't available for these so will cause additional wear on the disk.

## Supercard Pro at https://www.cbmstuff.com/
Requires firmware V1.3

# Contribute
If you have another device you would like to contribute to this or have a suggestion or improvement let me know/make a pull request.

RobSmithDev
https://www.patreon.com/RobSmithDev
https://amiga.robsmithdev.co.uk/winuae
https://www.youtube.com/c/robsmithdev
