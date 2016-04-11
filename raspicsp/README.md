# RaspICSP

Contains a utility written in C++ which uses three GPIOs of the raspberry to programm a connected PIC24 device. 
Right now only very few devices are supported, but others can be added easily. (See devices.h for further information).

## Running the tool

Compile it using
> cmake .
> make

Then invoke it:
> ./raspicsp PIC24FJ64GB0XX test.hex

## Architecture

A simple layered architecture is used. This should make to code quite portable to a) other ARM devices or b) other target devices like PIC18.

### HAL - Hardware Abstraction Layer

Contains all the raspberry related code to access GPIOs by mapping the respective registers of the BCM2708 controller in the local address space.
It exposes a simple API to pull the MCLR pin high and low and to read and write a bit (by reading/writing PGD and sending a clokc signal on PGC).

### ICSP - In-Circuit Serial Programmer

Contains the logic to put the device into ICSP mode (as specified in the Flash Programming Specification by Microchip). It also takes
care of sending op-codes (SIX commands) and reading the communication register (VISI).

### PIC24 - Execution Engine

Contains the actual machine code lisitings which erase the chip and reads or writes the configuration memory (those are also given by the Flash Programming Specification by Microchip).

### Misc

Logger.h / Logger.cpp contain a simple logging facility used by the other components. HexFile.h / HexFile.cpp contain
a simple reader for "Intel HEX Files". This is the format used by most (all?) tools including the C compilers from Microchip. Basically
it is a list of byte oriented data with the respective addresses. The main issue with its is to convert them back to 16 bit words and not to
turn insane by the 24bit addressing model of the PIC....
