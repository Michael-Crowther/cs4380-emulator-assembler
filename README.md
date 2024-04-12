Project 1 Emulator - Mark 1 of the 4380

This emulator accepts up to two command line arguments for reading in a binary file with an optional memory size. It then enters an execution loop to fetch, decode, and execute valid instructions specified by the ISA. I've written tests for these functions, which can be found in the src folder, and run with ./runTests after compiling Cmake and make. 


Project 2 Assembler

The 4380 assembler takes in a .asm file, converts the contents to binary, and outputs a binary file of the same name, with etension ".bin". I have a series of pytests that can be run using "python3 -m pytest myTests.py". It runs the test files located in the subdirectory of "assembler".

Project 3

INVOKING THE NON_LATE POLICY

WORK LOG FOR PROJECT-3 NON LATE POLICY
--------------------------------------

Monday, March 25: Worked on Cache base class, private and public methods. Started setting up strcture for derived classes. Hours worked - 2.0

Tuesday, March 26: Setup command line argument flags for optional memory size and cache type: Hours worked: 1.5

Wednesday, March 27: Changed Emulator and Assembler to accept the new instructions for emu4380 mark 2. Hours: 3.0 

Thursday, March 28: Implement Direct Mapped Cache. Hours Worked: 3.0

Saturday, March 30: Made sure Direct Mapped Cache can read and write more than one block of memory. Hours: 1.5

Sunday, March 31: Implemented Fully Associative Cache. Hours: 3.0

Tuesday, April 2: Started 2-way set associative cache. Hours: 1.5

Friday, April 5: Emulator is broken! I can't finish part 5 of the cache assignment until my emulator and assembler are working. Will need time to fix.

Hours worked before project due: 15.5

--------------------------------------
