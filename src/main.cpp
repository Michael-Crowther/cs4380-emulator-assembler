#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iomanip>
#include "../include/emu4380.h"

using namespace std;

int main(int argc, char** argv){
	if(argc < 2 || argc > 3){
		cout << "Enter at least one arg, no more than two" << endl;
		return 0;
	}

	string bytecodeFile = argv[1];

	if(argc == 3){
		char* endPtr;
		memorySize = strtoul(argv[2], &endPtr, 10);

		if(memorySize <= 0 || memorySize > 4294967295){
			cout << "INSUFFICIENT MEMORY SPACE\n";
			return 2;
		}
	}

	//allocate program memory
  init_mem(memorySize);

  //open binary file
  ifstream binaryFile(bytecodeFile, ios::binary);
  if(!binaryFile){
  	delete[] prog_mem;
    return 0;
  }


	//check if binaryFile size is larger than memorySize
	binaryFile.seekg(0, ios::end);
	unsigned long fileSize = binaryFile.tellg();
	binaryFile.seekg(0, ios::beg);

	if(fileSize > memorySize){
		cout << "INSUFFICIENT MEMORY SPACE\n";
    delete[] prog_mem;
    return 2;
	}	

  //read file into program memory
 	binaryFile.read(reinterpret_cast<char*>(prog_mem), memorySize);
	binaryFile.close();

	//set PC register here
	reg_file[RegNames::PC] = *reinterpret_cast<unsigned int*>(prog_mem);

	//execution loop
	bool running = true;
	while(running){
		if(!fetch() || !decode() || !execute()){
			cout << "INVALID INSTRUCTION AT: " << reg_file[RegNames::PC] << endl;
			return 1;
		}
	}

  delete[] prog_mem;
  return 1;
}
