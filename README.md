# OpenCobra
This is a collection of software and hardware which I build for my CNC (and maybe a 3D printer)

The main intention is to share Ideas and help others to overcome some pitfalls.

Eventually the project will consist of:
* A piece of software running on a Raspberry Pi (or the like) which provides a web interface to control the machine. 
* [A PIC24 based controller board](alig/README.md) which contains the stepper motor drivers, PWM controllers and some fancy control switches.
* The firmware which interprets the G-Code has the be built as well
* [An ICSP programmer](raspicsp/README.md) running on the Raspberry Pi which programs the PIC (yes that could be done using a bootloader, but
  ICSP via the Raspberry Pi can also do the initial programming without having to provide an additional ICSP socket on the already jammed board.

