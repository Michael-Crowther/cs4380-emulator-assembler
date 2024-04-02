#include <string>
#include <limits>
#include<iomanip>
#include <fstream>
#include <iostream>
#include <cstring>
#include <unordered_set>
#include <typeinfo>
#include "../include/emu4380.h"

using namespace std;

unsigned int reg_file[num_gen_regs];
unsigned char* prog_mem = nullptr;
unsigned int memorySize = 131072;
unsigned int mem_cycle_cntr = 0;
unsigned int cntrl_regs[num_cntrl_regs];
unsigned int data_regs[num_data_regs];
static const unordered_set<unsigned int> validOperations = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
Cache* globalCache = nullptr;

void init_cache(unsigned int cacheType){
  delete globalCache; //cleanup existing cache if any

  switch(cacheType){
    case 0:
      globalCache = new NoCache();
      break;
    case 1:
      globalCache = new DirectMappedCache();
      break;
    case 2:
      globalCache = new FullyAssociativeCache();
      break;
		case 3:
			globalCache = new TwoWaySetAssociativeCache();
			break;
    default:
      globalCache = new NoCache();
      break;
  }
}

//NoCache methods
unsigned char NoCache::readByte(unsigned int address) {
  mem_cycle_cntr += 8;
  return prog_mem[address];
}

unsigned int NoCache::readWord(unsigned int address) {
  mem_cycle_cntr += 8;
  return *reinterpret_cast<unsigned int*>(&prog_mem[address]);
}

void NoCache::writeByte(unsigned int address, unsigned char byte) {
  mem_cycle_cntr += 8;
  prog_mem[address] = byte;
}

void NoCache::writeWord(unsigned int address, unsigned int word){
  mem_cycle_cntr += 8;
  *reinterpret_cast<unsigned int*>(&prog_mem[address]) = word;
}
void NoCache::init(unsigned int cacheType) {
	mem_cycle_cntr = 0;
  return;
}



//DirectMappedCache methods
//constructor
DirectMappedCache::DirectMappedCache() : cacheLines(numLines) {}

//private methods
unsigned int DirectMappedCache::getTag(unsigned int address) const {
    return address / (blockSize * numLines);
}

unsigned int DirectMappedCache::getIndex(unsigned int address) const {
    return (address / blockSize) % numLines;
}

unsigned int DirectMappedCache::getBlockOffset(unsigned int address) const {
    return address % blockSize;
}

void DirectMappedCache::writeBack(unsigned int index) {
    auto& line = cacheLines[index];
    if (line.dirty) {
        unsigned int startAddress = (line.tag * numLines + index) * blockSize;
        std::memcpy(&prog_mem[startAddress], line.data.data(), blockSize);
        line.dirty = false;
        mem_cycle_cntr += 14; // Additional cycles for write-back
    }
}

void DirectMappedCache::loadFromMemory(unsigned int address, unsigned int index) {
    unsigned int startAddress = address - getBlockOffset(address);
    auto& line = cacheLines[index];
    writeBack(index); // Write back if needed
    std::memcpy(line.data.data(), &prog_mem[startAddress], blockSize);
    line.valid = true;
    line.tag = getTag(address);
}

//public methods
unsigned char DirectMappedCache::readByte(unsigned int address) {
  unsigned int index = getIndex(address);
	unsigned int tag = getTag(address);
	unsigned int offset = getBlockOffset(address);

	auto& line = cacheLines[index];
	if(line.valid && line.tag == tag){
		mem_cycle_cntr += 1; //Cache hit
		return line.data[offset];
	} 
	else {
		loadFromMemory(address, index);
		mem_cycle_cntr += 15;
		return line.data[offset]; //a hit after loading
	}
}

unsigned int DirectMappedCache::readWord(unsigned int address) {
	unsigned int value = 0;
  unsigned int startOffset = getBlockOffset(address);
  unsigned int endOffset = getBlockOffset(address + 3);//last byte of word

	//check if word spans two blocks
	if(startOffset <= 12 || startOffset == endOffset){
		for (int i = 0; i < 4; ++i) {
    	value |= static_cast<unsigned int>(readByte(address + i)) << (8 * i);
    }
	}
	else{
		//word spans two cache lines
		bool firstBlockLoaded = false;
		bool secondBlockLoaded = false;

		for (int i = 0; i < 4; ++i) {
    	// Determine if loading a block is necessary for each byte
    	unsigned int index = getIndex(address + i);
      unsigned int tag = getTag(address + i);

      if (!cacheLines[index].valid || cacheLines[index].tag != tag) {
      	if (i < 2) firstBlockLoaded = true; // First half of the word
        else secondBlockLoaded = true; // Second half of the word
          
        loadFromMemory(address + i, index); // Load necessary block
      }

      unsigned int offset = getBlockOffset(address + i);
      value |= static_cast<unsigned int>(cacheLines[index].data[offset]) << (8 * i);
    }

    // Adjust mem_cycle_cntr based on block loads
    if (firstBlockLoaded && !secondBlockLoaded) {
    	mem_cycle_cntr += 15; // Only the first block was loaded
    } else if (firstBlockLoaded && secondBlockLoaded) {
    	mem_cycle_cntr += 23; // Both blocks were loaded
    }
	}
	return value;
}

void DirectMappedCache::writeByte(unsigned int address, unsigned char byte) {
    unsigned int index = getIndex(address);
    unsigned int tag = getTag(address);
    unsigned int offset = getBlockOffset(address);

    auto& line = cacheLines[index];
    if (!(line.valid && line.tag == tag)) {
    	loadFromMemory(address, index); // Ensures correct block is loaded
			mem_cycle_cntr += 15;	
    }
    line.data[offset] = byte;
    line.dirty = true;
    mem_cycle_cntr += 1; // Considered a cache hit
}

void DirectMappedCache::writeWord(unsigned int address, unsigned int word){
	unsigned int startOffset = getBlockOffset(address);
  unsigned int endOffset = getBlockOffset(address + 3); // Last byte of the word

  // Check if word spans two blocks
  if (startOffset <= 12 || startOffset == endOffset) {
  	for (int i = 0; i < 4; ++i) {
    	writeByte(address + i, static_cast<unsigned char>((word >> (8 * i)) & 0xFF));
     }
    } 
	else {
  	// Word spans two cache lines
    bool firstBlockLoaded = false;
    bool secondBlockLoaded = false;

    for (int i = 0; i < 4; ++i) {
    	unsigned int currentAddress = address + i;
      unsigned int index = getIndex(currentAddress);
      unsigned int tag = getTag(currentAddress);

      // Check if the cache line needs to be loaded or is already present
      if (!cacheLines[index].valid || cacheLines[index].tag != tag) {
      	// Determine which half of the word we're in
        if (i < 2) firstBlockLoaded = true;
        else secondBlockLoaded = true;

        loadFromMemory(currentAddress, index); // Load necessary block
      }

      unsigned int offset = getBlockOffset(currentAddress);
      cacheLines[index].data[offset] = static_cast<unsigned char>((word >> (8 * i)) & 0xFF);
      cacheLines[index].dirty = true; // Mark the line as dirty since we've written to it
    }

    if (firstBlockLoaded && !secondBlockLoaded) {
    	mem_cycle_cntr += 15; // Only the first block was loaded
    } else if (firstBlockLoaded && secondBlockLoaded) {
      mem_cycle_cntr += 23; // Both blocks were loaded
    }
 }
}

void DirectMappedCache::init(unsigned int cacheType) {
  for (auto& line : cacheLines) {
        line.valid = false;
        line.dirty = false;
    }
}




//FullyAssociativeCache methods
//constructor
FullyAssociativeCache::FullyAssociativeCache() : cacheLines(numLines) {}

//private methods
unsigned int FullyAssociativeCache::calculateTag(unsigned int address) const {
    return address / blockSize;
}

void FullyAssociativeCache::accessLine(unsigned int tag) {
    // If tag exists in LRU map, move it to the front of the LRU list
    if (lruMap.find(tag) != lruMap.end()) {
        lruList.splice(lruList.begin(), lruList, lruMap[tag]);
        lruMap[tag] = lruList.begin();
    }
}

void FullyAssociativeCache::evictIfNeeded() {
    if (lruList.size() >= numLines) {
        // Evict the least recently used line
        unsigned int tagToEvict = lruList.back();
        writeBack(tagToEvict);
        // Remove from LRU tracking
        lruList.pop_back();
        lruMap.erase(tagToEvict);
    }
}

void FullyAssociativeCache::loadLine(unsigned int address, unsigned int tag) {
    evictIfNeeded();
    // Find an invalid line or just use the one corresponding to the evicted tag
    for (auto& line : cacheLines) {
        if (!line.valid || line.tag == tag) {
            std::memcpy(line.data.data(), &prog_mem[address - (address % blockSize)], blockSize);
            line.valid = true;
            line.dirty = false;
            line.tag = tag;
            // Update LRU
            lruList.push_front(tag);
            lruMap[tag] = lruList.begin();
            break;
        }
    }
}

void FullyAssociativeCache::writeBack(unsigned int tag) {
	for (auto& line : cacheLines) {
  	if (line.valid && line.dirty && line.tag == tag) {
    	// address to write back to
      unsigned int address = line.tag * blockSize;
      // write back to memory
      std::memcpy(&prog_mem[address], line.data.data(), blockSize);
      line.dirty = false;
      mem_cycle_cntr += 14; // write-back penalty
      break;
    }
  }
}

unsigned char FullyAssociativeCache::readByte(unsigned int address) {
	unsigned int tag = calculateTag(address);
  unsigned int offset = address % blockSize;

  // Search for the tag in the cache
  for (auto& line : cacheLines) {
      if (line.valid && line.tag == tag) {
          // hit - update LRU
          accessLine(tag);
          mem_cycle_cntr += 1;
          return line.data[offset];
      }
  }

  // miss - load the line into the cache
  loadLine(address, tag);
  mem_cycle_cntr += 15; // miss penalty
  for (auto& line : cacheLines) {
  	if (line.valid && line.tag == tag) {
    	return line.data[offset];
    	}
  }

	throw std::runtime_error("Cache line not found after loading");
}

unsigned int FullyAssociativeCache::readWord(unsigned int address) {
	unsigned int word = 0;
  bool firstBlockMiss = false;
  bool secondBlockMiss = false;

  for (int i = 0; i < 4; ++i) {
  	unsigned int currentAddress = address + i;
    unsigned int tag = calculateTag(currentAddress);

    // Check if the block for this byte is in the cache
    auto it = lruMap.find(tag);
    if (it == lruMap.end()) {
    	// Cache miss
      if (i < 2) firstBlockMiss = true; // First half of the word
      else secondBlockMiss = true; // Second half of the word
    }

    word |= static_cast<unsigned int>(readByte(currentAddress)) << (8 * i);
  }

    if (firstBlockMiss && secondBlockMiss) {
        mem_cycle_cntr += 8; // Only add the additional cycles for the second block
    }

    return word;
}

void FullyAssociativeCache::writeByte(unsigned int address, unsigned char byte) {
	unsigned int tag = calculateTag(address);
  unsigned int offset = address % blockSize;

  // Check if the tag exists in the cache
  for (auto& line : cacheLines) {
      if (line.valid && line.tag == tag) {
          // hit - update LRU and mark as dirty
          accessLine(tag);
          line.data[offset] = byte;
          line.dirty = true;
          mem_cycle_cntr += 1;
          return;
      }
  }

  // miss - load the line into the cache and write the byte
  loadLine(address, tag);
  mem_cycle_cntr += 15; //miss penalty
	
	// After loading, find the line again to write the byte
  for (auto& line : cacheLines) {
  	if (line.valid && line.tag == tag) {
    	line.data[offset] = byte;
      line.dirty = true;
      return;
    }
  }
}

void FullyAssociativeCache::writeWord(unsigned int address, unsigned int word){
	bool firstBlockMiss = false;
  bool secondBlockMiss = false;

  for (int i = 0; i < 4; ++i) {
  	unsigned int currentAddress = address + i;
    unsigned int tag = calculateTag(currentAddress);

    auto it = lruMap.find(tag);
    if (it == lruMap.end()) {
    	if (i < 2) firstBlockMiss = true; // First half of the word indicates first block
      else secondBlockMiss = true; // Second half of the word indicates potential second block
    }

    // Perform the byte write operation
    writeByte(currentAddress, static_cast<unsigned char>((word >> (8 * i)) & 0xFF));
  }

  if (firstBlockMiss && secondBlockMiss) {
  	mem_cycle_cntr += 8; // Only add the additional cycles for the second block
  }
}
void FullyAssociativeCache::init(unsigned int cacheType) {
    for (auto& line : cacheLines) {
      line.valid = false;
      line.dirty = false;
    }
    lruList.clear();
    lruMap.clear();
}



//TwoWaySetAssociativeCache
//constructor
TwoWaySetAssociativeCache::TwoWaySetAssociativeCache() : cacheSets(numSets) {}

//private methods
unsigned int TwoWaySetAssociativeCache::getTag(unsigned int address) const {
	return address / (blockSize * numSets);
}

unsigned int TwoWaySetAssociativeCache::getSetIndex(unsigned int address) const {
  return (address / blockSize) % numSets;
}

unsigned int TwoWaySetAssociativeCache::getBlockOffset(unsigned int address) const {
  return address % blockSize;
}

void TwoWaySetAssociativeCache::updateLRU(unsigned int setIndex, unsigned int accessedLine) {
	cacheSets[setIndex].lruFlags[accessedLine] = true;  
  cacheSets[setIndex].lruFlags[1 - accessedLine] = false;
}

void TwoWaySetAssociativeCache::writeBack(unsigned int setIndex, unsigned int lineIndex) {
	auto& line = cacheSets[setIndex].lines[lineIndex];
  if (line.valid && line.dirty) {
		mem_cycle_cntr += 14; //write back penalty
  	unsigned int baseAddress = (line.tag * numSets + setIndex) * blockSize;
    for (unsigned int i = 0; i < blockSize; ++i) {
    // don't exceed the memorySize
    	if ((baseAddress + i) < memorySize) {
    		prog_mem[baseAddress + i] = line.data[i];
    	}
  	}
   line.dirty = false; // Clear the dirty bit after writing back
  }
}

void TwoWaySetAssociativeCache::loadFromMemory(unsigned int address, unsigned int setIndex, unsigned int lineIndex) {
	auto& line = cacheSets[setIndex].lines[lineIndex];
  unsigned int tag = getTag(address);
  unsigned int baseAddress = (tag * numSets + setIndex) * blockSize;
	mem_cycle_cntr += 15;

  for (unsigned int i = 0; i < blockSize; ++i) {
  	// don't exceed the memorySize
  	if ((baseAddress + i) < memorySize) {
    	line.data[i] = prog_mem[baseAddress + i];
    }
  }

	line.valid = true;
  line.dirty = false;
  line.tag = tag;
}

unsigned char TwoWaySetAssociativeCache::readByte(unsigned int address) {
	unsigned int tag = getTag(address);
  unsigned int setIndex = getSetIndex(address);
  unsigned int blockOffset = getBlockOffset(address);

  for (unsigned int i = 0; i < 2; ++i) {
      auto& line = cacheSets[setIndex].lines[i];
      if (line.valid && line.tag == tag) {
          // Cache hit
          updateLRU(setIndex, i);
          mem_cycle_cntr += 1; // Hit penalty
          return line.data[blockOffset];
      }
  }

  // Cache miss
  unsigned int lineToUse = cacheSets[setIndex].lruFlags[0] ? 0 : 1; // Use the LRU line
  if (cacheSets[setIndex].lines[lineToUse].valid && cacheSets[setIndex].lines[lineToUse].dirty) {
  	writeBack(setIndex, lineToUse);
  }
  loadFromMemory(address, setIndex, lineToUse);
  updateLRU(setIndex, lineToUse);
  return cacheSets[setIndex].lines[lineToUse].data[blockOffset];
}

void TwoWaySetAssociativeCache::writeByte(unsigned int address, unsigned char byte) {
	unsigned int tag = getTag(address);
  unsigned int setIndex = getSetIndex(address);
  unsigned int blockOffset = getBlockOffset(address);

  for (unsigned int i = 0; i < 2; ++i) {
  	auto& line = cacheSets[setIndex].lines[i];
    if (line.valid && line.tag == tag) {
    	// Cache hit
      line.data[blockOffset] = byte;
      line.dirty = true;
      updateLRU(setIndex, i);
      mem_cycle_cntr += 1; // Hit penalty
      return;
    }
  }

  // Cache miss, select LRU line for writing
  unsigned int lineToUse = cacheSets[setIndex].lruFlags[0] ? 0 : 1;
  if (cacheSets[setIndex].lines[lineToUse].valid && cacheSets[setIndex].lines[lineToUse].dirty) {
  	writeBack(setIndex, lineToUse);
  }
  loadFromMemory(address, setIndex, lineToUse); // Load the block first
  cacheSets[setIndex].lines[lineToUse].data[blockOffset] = byte;
  cacheSets[setIndex].lines[lineToUse].dirty = true;
  updateLRU(setIndex, lineToUse);
}

unsigned int TwoWaySetAssociativeCache::readWord(unsigned int address) {
	unsigned int word = 0;
  unsigned int startOffset = getBlockOffset(address);
  unsigned int endOffset = getBlockOffset(address + 3); //last byte of the word.

  bool firstBlockMiss = false;
  bool secondBlockMiss = false;

  for (int i = 0; i < 4; ++i) {
  	unsigned int currentAddress = address + i;
    unsigned int setIndex = getSetIndex(currentAddress);
    unsigned int tag = getTag(currentAddress);
    unsigned int blockOffset = getBlockOffset(currentAddress);

    // Attempt to read the byte from the cache, checking both lines in the set.
    bool foundInCache = false;
    for (unsigned int j = 0; j < 2; ++j) {
    	auto& line = cacheSets[setIndex].lines[j];
      if (line.valid && line.tag == tag) {
      	// Cache hit
        foundInCache = true;
        updateLRU(setIndex, j);
        word |= static_cast<unsigned int>(line.data[blockOffset]) << (i * 8);
        break;
      }
   }

     if (!foundInCache) {
     	// Cache miss for this part of the word.
      if (i < 2) firstBlockMiss = true; // Miss in the first half of the word.
      else secondBlockMiss = true; // Miss in the second half of the word.

      unsigned int lineToUse = cacheSets[setIndex].lruFlags[0] ? 1 : 0;
      loadFromMemory(currentAddress, setIndex, lineToUse);
      updateLRU(setIndex, lineToUse);

      // After loading, read the byte from the newly loaded line.
      auto& newLine = cacheSets[setIndex].lines[lineToUse];
      word |= static_cast<unsigned int>(newLine.data[blockOffset]) << (i * 8);
    }
  }

  if (firstBlockMiss && !secondBlockMiss) {
  	mem_cycle_cntr += 15; // Only the first block was loaded.
  } else if (firstBlockMiss && secondBlockMiss) {
  	mem_cycle_cntr += 23; // Both blocks were loaded
  }

    return word;
}

void TwoWaySetAssociativeCache::writeWord(unsigned int address, unsigned int word) {
	unsigned int startOffset = getBlockOffset(address);
  unsigned int endOffset = getBlockOffset(address + 3); // Last byte of the word.

  // Determine if the operation potentially spans two blocks within a set.
  if (startOffset <= (blockSize - 4) || startOffset == endOffset) {
  	//  word does not span across two blocks.
    for (int i = 0; i < 4; ++i) {
    	writeByte(address + i, static_cast<unsigned char>((word >> (i * 8)) & 0xFF));
    }
  }
	else {
		// word spans across two blocks.
    bool firstBlockAffected = false;
    bool secondBlockAffected = false;
 
	  for (int i = 0; i < 4; ++i) {
    	unsigned int currentAddress = address + i;
      unsigned int setIndex = getSetIndex(currentAddress);
      unsigned int tag = getTag(currentAddress);
            
      // Determine if this part of the word affects the first or second block.
      bool isCurrentBlockLoaded = false;
      for (unsigned int j = 0; j < 2; ++j) {
      	auto& line = cacheSets[setIndex].lines[j];
       	if (line.valid && line.tag == tag) {
        	isCurrentBlockLoaded = true;
          break; // The current block is already loaded in the cache.
        }
      }

      if (!isCurrentBlockLoaded) {
      	if (i < 2) firstBlockAffected = true;
        else secondBlockAffected = true;
      }

      writeByte(currentAddress, static_cast<unsigned char>((word >> (i * 8)) & 0xFF));
   }

   if (firstBlockAffected && secondBlockAffected) {
   	 mem_cycle_cntr += 8; // Additional cycles for the second block.
   }
  }
}

void TwoWaySetAssociativeCache::init(unsigned int cacheType) {
	for(auto& set : cacheSets){
		for(auto& line : set.lines){
			line.valid = false;
			line.dirty = false;	
			line.tag = 0;
			line.data.fill(0);
		}
		set.lruFlags = {false, false};
	}
}



bool isValidRegister(unsigned int reg){
  return reg < num_gen_regs;
};

bool init_mem(unsigned int size){
  prog_mem = new unsigned char[size];
  if(prog_mem){
    std::memset(prog_mem, 0, size);
    return true;
  }
  return false;
}

bool fetch(){
  if(reg_file[PC] >= memorySize || reg_file[PC] + 8 > memorySize){
    return false;
  }

  unsigned int firstWord = globalCache->readWord(reg_file[PC]);
  cntrl_regs[OPERATION] = firstWord & 0xFF;
  cntrl_regs[OPERAND_1] = (firstWord >> 8) & 0xFF;
  cntrl_regs[OPERAND_2] = (firstWord >> 16) & 0xFF;
  cntrl_regs[OPERAND_3] = (firstWord >> 24) & 0xFF;

  cntrl_regs[IMMEDIATE] = globalCache->readWord(reg_file[PC] + 4);

  if (dynamic_cast<NoCache*>(globalCache) != nullptr) {
		mem_cycle_cntr -= 6;
  }

  //move to next instruction
  reg_file[RegNames::PC] += 8;
  return true;
}


bool decode(){
	//cout << "decode" << endl;
	unsigned int operation = cntrl_regs[OPERATION];
  unsigned int operand1 = cntrl_regs[OPERAND_1];
  unsigned int operand2 = cntrl_regs[OPERAND_2];
  unsigned int operand3 = cntrl_regs[OPERAND_3];

	//check validity of operation
	if(validOperations.find(operation) == validOperations.end()){
		return false;
	}

	switch(operation){
		case 1: //JMP
			break;
		case 2: //JMR
		case 3: //BNZ
		case 4: //BGT
		case 5: //BLT
			if (!isValidRegister(operand1)) return false;
      data_regs[REG_VAL_1] = reg_file[operand1];
      break;
		case 7: //MOV	
		case 14: //ISTR
		case 15: //ILDR
		case 16: //ISTB
		case 17: //ILDB
			if(!isValidRegister(operand1) || !isValidRegister(operand2)) return false;
			data_regs[REG_VAL_1] = reg_file[operand1]; //RD
			data_regs[REG_VAL_2] = reg_file[operand2]; //RS
			break;
		case 8: //MOVI
		case 9: //LDA
		case 10: //STR
		case 11: //LDR
		case 12: //STB
		case 13: //LDB
		case 18: //ADD
		case 20: //SUB
		case 22: //MUL
		case 24: //DIV
		case 25: //SDIV
		case 19: //ADDI
		case 21: //SUBI
		case 23: //MULI
		case 26: //DIVI
		case 29: //CMP
		case 30: //CMPI
			if (!isValidRegister(operand1)) return false;
      data_regs[REG_VAL_1] = reg_file[operand1];
      break;
		case 31: //TRP
			data_regs[REG_VAL_1] = operand1;
			break;
		default:
			return false;
			break;
	}	
	return true;
}

bool execute(){
        unsigned int operation = cntrl_regs[OPERATION];
        unsigned int operand1 = cntrl_regs[OPERAND_1];
        unsigned int operand2 = cntrl_regs[OPERAND_2];
        unsigned int operand3 = cntrl_regs[OPERAND_3];
        unsigned int immediate = cntrl_regs[IMMEDIATE];

        switch(operation){
                case 1: //JMP
                        reg_file[PC] = immediate; //jump to address of immediate
                        break;
								case 2: //JMR
												reg_file[PC] = reg_file[operand1];
												break;
								case 3: //BNZ
												if(reg_file[operand1] != 0){
													reg_file[PC] = immediate;
												}
												break;
								case 4: //BGT
												if(reg_file[operand1] > 0){
                          reg_file[PC] = immediate;
                        }
                        break;
								case 5: //BLT
												if(reg_file[operand1] < 0){
                          reg_file[PC] = immediate;
                        }
                        break;
                case 7: //MOV
                        reg_file[operand1] = reg_file[operand2]; //move contents of RS to RD
                        break;
                case 8: //MOVI
                        reg_file[operand1] = immediate; //move immediate value into RD
                        break;
                case 9: //LDA
                        reg_file[operand1] = immediate; //load address of immediate into RD
                        break;
                case 10: //STR
                        if(immediate >= memorySize) return false;
												globalCache->writeWord(immediate, reg_file[operand2]);
                        break;
                case 11: //LDR
                        if(immediate >= memorySize) return false;
												reg_file[operand1] = globalCache->readWord(immediate);
                        break;
                case 12: //STB
                        if(immediate >= memorySize) return false;
												globalCache->writeByte(immediate, static_cast<unsigned char>(reg_file[operand2] & 0xFF)); 
                        break;
                case 13: //LDB
                        if(immediate >= memorySize) return false;
												reg_file[operand1] = globalCache->readByte(immediate);
                        break;
								case 14: // ISTR
    										{
        									unsigned int address = reg_file[operand2];
        									if(address >= memorySize) return false;
													globalCache->writeWord(address, reg_file[operand1]);
    										}
    										break;
								case 15: // ILDR
    										{
        									unsigned int address = reg_file[operand2];
       	 									if(address >= memorySize) return false;
													reg_file[operand1] = globalCache->readWord(address);
    										}
    										break;
								case 16: // ISTB
    										{
        									unsigned int address = reg_file[operand2];
        									if(address >= memorySize) return false;
													globalCache->writeByte(address, static_cast<unsigned char>(reg_file[operand1] & 0xFF)); // Store LS byte of RS at RG address
    										}
    										break;
								case 17: // ILDB
    										{
        									unsigned int address = reg_file[operand2];
        									if(address >= memorySize) return false;
													reg_file[operand1] = globalCache->readByte(address);
    										}
    										break;
                case 18: //ADD
                        reg_file[operand1] = reg_file[operand2] + reg_file[operand3]; //add RS1 to RS2, sotre in RD
                        break;
                case 19: //ADDI
                        reg_file[operand1] = reg_file[operand2] + immediate; //Add Imm to RS1, store in RD
                        break;
                case 20: //SUB
                        reg_file[operand1] = reg_file[operand2] - reg_file[operand3]; //subtract RS2 from RS1, store result in RD
                        break;
                case 21: //SUBI
                        reg_file[operand1] = reg_file[operand2] - immediate; //subtract imm from RS1 store in RD
                        break;
                case 22: //MUL
                        reg_file[operand1] = reg_file[operand2] * reg_file[operand3]; //multiply RS1 by RS2, store in RD
                        break;
                case 23: //MULI
                        reg_file[operand1] = reg_file[operand2] * immediate; //multiply RS1 by immediate, store in RD
                        break;
                case 24: //DIV
                        if(reg_file[operand3] == 0) return false; //division by 0
                        reg_file[operand1] = reg_file[operand2] / reg_file[operand3]; //unsigned int division RS1/RS2, store in RD
                        break;
                case 25: //SDIV
                        if(reg_file[operand3] == 0) return false;
                        reg_file[operand1] = static_cast<unsigned int>(static_cast<int>(reg_file[operand2]) / static_cast<int>(reg_file[operand3])); //signed division of RS1/RS2
                        break;
                case 26: //DIVI
                        if(immediate == 0) return false;
                        reg_file[operand1] = static_cast<unsigned int>(static_cast<int>(reg_file[operand2]) / static_cast<int>(immediate)); //Divide RS1 by Imm (signed), result in RD.
                        break;
								case 29: //CMP
												if(reg_file[operand2] == reg_file[operand3]){
													reg_file[operand1] = 0;
												}
												else if(reg_file[operand2] > reg_file[operand3]){
														reg_file[operand1] = 1;
												}
												else if(reg_file[operand2] < reg_file[operand3]){
                            reg_file[operand1] = -1;
                        }
												break;
								case 30: //CMPI
												if(reg_file[operand2] == immediate){
													reg_file[operand1] = 0;
												}
												else if(reg_file[operand2] > immediate){
													reg_file[operand1] = 1;
												}
												else if(reg_file[operand2] < immediate){
                          reg_file[operand1] = -1;
                        }
												break;
                case 31: //TRP
                        switch(operand1){
                                case 0: //STOP-Exit
																				cout << "Execution completed. Total memory cycles: " << mem_cycle_cntr << endl;
																				delete[] prog_mem;
																				delete globalCache;
                                        exit(0);
                                        break;
                                case 1: //Write int in R3 to stdout
                                        cout << reg_file[R3];
                                        break;
                                case 2: //Read an integer into R3 from stdin
                                        cin >> reg_file[R3];
                                        break;
                                case 3: //Write char in R3 to stdout
                                        cout << static_cast<char>(reg_file[R3]);
                                        break;
                                case 4: //Read a char into R3 from stdin
                                        reg_file[R3] = static_cast<unsigned int>(cin.get());
                                        break;
                                case 98: //Print all registers to stdout
                                        for(size_t i = 0; i < num_gen_regs; ++i){
                        // Print general-purpose registers
                        cout << "R" << i << "\t" << reg_file[i] << endl;
                        }
                        // Printing special registers with their names in all caps as per requirement
                        cout << "PC\t" << reg_file[PC] << endl;
                        cout << "SL\t" << reg_file[SL] << endl;
                        cout << "SB\t" << reg_file[SB] << endl;
                        cout << "SP\t" << reg_file[SP] << endl;
                        cout << "FP\t" << reg_file[FP] << endl;
                        cout << "HP\t" << reg_file[HP] << endl;
                                        break;
                                default:
                                        return false;
                        }
                break;
        }
        return true;
}
