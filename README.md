Project 1 Emulator - Mark 1 of the 4380

This emulator accepts up to two command line arguments for reading in a binary file with an optional memory size. It then enters an execution loop to fetch, decode, and execute valid instructions specified by the ISA. I've written tests for these functions, which can be found in the src folder, and run with ./runTests after compiling Cmake and make. 


Project 2 Assembler

The 4380 assembler takes in a .asm file, converts the contents to binary, and outputs a binary file of the same name, with etension ".bin". I have a series of pytests that can be run using "python3 -m pytest myTests.py". It runs the test files located in the subdirectory of "assembler".
