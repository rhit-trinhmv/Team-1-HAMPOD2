# NanoPi Firmware Code 
The code in this directory is for firmware for the hardware of the Hampod. For now, this includes the numpad and a way to communicate with Festival.
## Required Dependencies 
Everything needed to run this code is included in the nanoPi_install.sh script (don't forget to run as a root user).

The things needed are:
* alsa/alsa-mixer from libasound2 
* git
* festival
* festival-dev
* WiringNP repo. Link to [WiringNP Repo](https://github.com/friendlyarm/WiringNP)

## Programs
There are three programs in this directory. <br>
test - This program blinks an LED (On the pin GPIOG11) <br>
test2 - This program executes a bash command to Festival before blinking the same LED as in test. <br>
numpadtest - This program reads in button presses from the keypad and speaks them out on festival.

To run these programs, use the bash command `sudo ./numpadtest`(this is an example command for numpadtest). These programs must be run as root as the WiringNP library needs root access for the GPIO.

## Troubleshooting
If the programs are running into permission issues, use a sudo command to run the program. <br>
If the programs are missing dependencies, try running the nanoPi_install.sh to install everything needed for these programs. <br>
If the programs do not run after issuing the command `chmod +x filename` where filename is the name of the program/script you want to run.
