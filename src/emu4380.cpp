#include <string>
#include <limits>
#include <fstream>
#include <iostream>
#include <cstring>
#include <unordered_set>
#include "../include/emu4380.h"

using namespace std;

unsigned int reg_file[num_gen_regs];
unsigned char* prog_mem = nullptr;
unsigned int memorySize = 131072;
unsigned int cntrl_regs[num_cntrl_regs];
unsigned int data_regs[num_data_regs];
static const unordered_set<unsigned int> validOperations = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

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

	//1 byte operation, 3 bytes operands, 4 bytes immediate
	cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
  cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
  cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
  cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];

	cntrl_regs[CntrlRegNames::IMMEDIATE] =
      prog_mem[reg_file[PC] + 4] | 
			(prog_mem[reg_file[PC] + 5] << 8) | 
			(prog_mem[reg_file[PC] + 6] << 16) | 
			(prog_mem[reg_file[PC] + 7] << 24);	

	//move to next instruction
	reg_file[RegNames::PC] += 8;
	return true;
} 


bool decode(){
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
		case 7: //MOV
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
			*reinterpret_cast<unsigned int*>(prog_mem + immediate) = reg_file[operand2]; //store integer in RS at address immediate
			break;
		case 11: //LDR
			if(immediate >= memorySize) return false;
			reg_file[operand1] = *reinterpret_cast<unsigned int*>(prog_mem + immediate); //load integer at memory address into RD
			break;
		case 12: //STB
			if(immediate >= memorySize) return false;
			prog_mem[immediate] = static_cast<unsigned char>(reg_file[operand2] & 0xFF); //store least significant byte in RS at Address
			break;
		case 13: //LDB
			if(immediate >= memorySize) return false;
			reg_file[operand1] = prog_mem[immediate]; //load byte at address into RD
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
		case 31: //TRP
			switch(operand1){
				case 0: //STOP-Exit
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


