#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iomanip>
#include "../include/emu4380.h"

using namespace std;

int main(int argc, char** argv){
	if(argc < 2){
    cout << "Usage: " << argv[0] << " <input.bin> [-m memorySize] [-c cacheType]" << endl;
    return 0;
  }

  string bytecodeFile;
  unsigned long memorySize = 131072;
  unsigned int cacheType = 0;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-m" && i + 1 < argc) {
            char* endPtr;
            memorySize = strtoul(argv[i + 1], &endPtr, 10);
            if (memorySize <= 0 || memorySize > 4294967295) {
                cout << "INSUFFICIENT MEMORY SPACE\n";
                return 2;
            }
            i++;
        } else if (string(argv[i]) == "-c" && i + 1 < argc) {
            cacheType = atoi(argv[i + 1]);
            if (cacheType < 0 || cacheType > 3) {
                cout << "Invalid cache configuration. Aborting.\n";
                return 2;
            }
            i++;
        } else if (bytecodeFile.empty()) {
            bytecodeFile = argv[i];
        }
    }

  //allocate program memory
  init_mem(memorySize);
	init_cache(cacheType);

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
	cout << "got here" << endl;
	if(globalCache != nullptr){
		delete globalCache;
		globalCache = nullptr;
	}
  return 1;
}
