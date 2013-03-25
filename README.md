MIPS Emulator Prototype
==========

I wrote this on a plane flying back to MA from Stanford in 2010.  Was mostly annoyed at the incapability of the stock emulator for EE108B (computer architecture) from running the graphics and IO at the same time - so I wrote an emulator from scratch.

It seems to run at over 100MHz on my 3.5GHz Core i7 - your mileage may vary

USAGE
===
The project takes in a Xilinx COE file (ROM images used for the class) and converts that to a .BIN file which is little more than the raw instruction bitstream (NOTE: any big endian instruction bitstream should work).
./coetobin file.coe file.bin

To run the emulator:
./mipsemu file.bin

NOTE: unlike the actual emulator this emulator additionally has mouse input support.
