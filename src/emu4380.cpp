#include <string>
#include <limits>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include "../include/emu4380.h"

using namespace std;

unsigned int reg_file[num_gen_regs];
unsigned char* prog_mem;
unsigned int memorySize;
unsigned int cntrl_regs[num_cntrl_regs];
unsigned int data_regs[num_data_regs];
static const unordered_set<unsigned int> validOperations = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

bool isValidRegister(unsigned int reg){
  return reg < num_gen_regs;
};

bool fetch(){
	unsigned int PC = reg_file[static_cast<int>(RegNames::PC)];
	if(PC >= memorySize || PC + 8 > memorySize){
		return false;
	}

	//1 byte operation, 3 bytes operands, 4 bytes immediate
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = prog_mem[PC];
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)] = prog_mem[PC + 1];
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)] = prog_mem[PC + 2];
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_3)] = prog_mem[PC + 3];

	unsigned int immediateValue = 0;
	for (int i = 0; i < 4; ++i) {
    immediateValue |= static_cast<unsigned int>(prog_mem[PC + 4 + i]) << (i * 8);
	}
	cntrl_regs[static_cast<int>(CntrlRegNames::IMMEDIATE)] = immediateValue;

	//move to next instruction
	reg_file[static_cast<int>(RegNames::PC)] += 8;
	return true;
} 


bool decode(){
	unsigned int operation = cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)];
  unsigned int operand1 = cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)];
  unsigned int operand2 = cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)];
  unsigned int operand3 = cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_3)];
  unsigned int immediate = cntrl_regs[static_cast<int>(CntrlRegNames::IMMEDIATE)];

	//check validity of operation
	if(validOperations.find(operation) == validOperations.end()){
		return false;
	}

	switch(operation){
		case 1: //JMP
			data_regs[0] = immediate;
			break;
		case 7: //MOV
			if(!isValidRegister(operand1) || !isValidRegister(operand2)) return false;
			data_regs[0] = reg_file[operand1]; 
			data_regs[1] = reg_file[operand2]; 
			break;
		case 8: //MOVI
			if(!isValidRegister(operand1)) return false;
			data_regs[0] = reg_file[operand1];
			data_regs[1] = immediate; 
			break;
		case 9: //LDA
		case 10: //STR
		case 11: //LDR
		case 12: //STB
		case 13: //LDB
      if(!isValidRegister(operand1)) return false;
      data_regs[0] = reg_file[operand1];
      data_regs[1] = immediate;
      break;
		case 18: //ADD
		case 20: //SUB
		case 22: //MUL
		case 24: //DIV
		case 25: //SDIV
			if(!isValidRegister(operand1) || !isValidRegister(operand2) || !isValidRegister(operand3)) return false;
			data_regs[0] = reg_file[operand1];
			data_regs[1] = reg_file[operand2];
			data_regs[2] = reg_file[operand3];
			break;
		case 19: //ADDI
		case 21: //SUBI
		case 23: //MULI
		case 26: //DIVI
			if (!isValidRegister(operand1) || !isValidRegister(operand2)) return false;
			data_regs[0] = reg_file[operand1];
			data_regs[1] = reg_file[operand2];
			data_regs[2] = immediate;
			break;
		case 31: //TRP
			if (!isValidRegister(operand1)) return false;
			data_regs[0] = reg_file[operand1];
			break;
		default:
			return false;
			break;
	}	
	return true;
}


bool execute(){
	unsigned int operation = cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)];

	switch(operation){
		case 1: //JMP
			reg_file[static_cast<int>(RegNames::PC)] = data_regs[0];
			break;
		case 7: //MOV
			reg_file[data_regs[0]] = reg_file[data_regs[1]];
			break;
		case 8: //MOVI
			reg_file[data_regs[0]] = data_regs[1];
			break;
		case 9: //LDA
			reg_file[data_regs[0]] = data_regs[1];
			break;
		case 10: //STR
			if(data_regs[1] >= memorySize) return false;
			*reinterpret_cast<unsigned int*>(prog_mem + data_regs[1]) = reg_file[data_regs[0]]; //store integer in RS at address
			break;
		case 11: //LDR
			if(data_regs[1] >= memorySize) return false;
			reg_file[data_regs[0]] = *reinterpret_cast<unsigned int*>(prog_mem + data_regs[1]); //load integer at Address into RD
			break;
		case 12: //STB
			if(data_regs[1] >= memorySize) return false;
			prog_mem[data_regs[1]] = static_cast<unsigned char>(reg_file[data_regs[0]] & 0xFF); //store least significant byte in RS at Address
			break;
		case 13: //LDB
			if(data_regs[1] >= memorySize) return false;
			reg_file[data_regs[0]] = prog_mem[data_regs[1]]; //load byte at address into RD
			break;
		case 18: //ADD
			reg_file[data_regs[0]] = reg_file[data_regs[1]] + reg_file[data_regs[2]];
			break;
		case 19: //ADDI
			reg_file[data_regs[0]] = reg_file[data_regs[1]] + data_regs[2];
			break;
		case 20: //SUB
			reg_file[data_regs[0]] = reg_file[data_regs[1]] - reg_file[data_regs[2]];
			break;
		case 21: //SUBI
			reg_file[data_regs[0]] = reg_file[data_regs[1]] - data_regs[2];
			break;
		case 22: //MUL
			reg_file[data_regs[0]] = reg_file[data_regs[1]] * reg_file[data_regs[2]];
			break;
		case 23: //MULI
			reg_file[data_regs[0]] = reg_file[data_regs[1]] * data_regs[2];
			break;
		case 24: //DIV
			if(reg_file[data_regs[2]] == 0) return false; //division by 0
			reg_file[data_regs[0]] = reg_file[data_regs[1]] / reg_file[data_regs[2]];
			break;
		case 25: //SDIV
			if(reg_file[data_regs[2]] == 0) return false;
			reg_file[data_regs[0]] = static_cast<int>(reg_file[data_regs[1]]) / static_cast<int>(reg_file[data_regs[2]]);
			break;
		case 26: //DIVI
			if (data_regs[2] == 0) return false;
			reg_file[data_regs[0]] = reg_file[data_regs[1]] / data_regs[2];
			break;
		case 31: //TRP
			switch(reg_file[0]){
				case 0: //STOP-Exit
					exit(0);
					break;
				case 1: //Write int in R3 to stdout
					cout << reg_file[3] << endl;
					break;
				case 2: //Read an integer into R3 from stdin
					cin >> reg_file[3];
					break;
				case 3: //Write char in R3 to stdout
					cout << static_cast<char>(reg_file[3]) << endl;
					break;
				case 4: //Read a char into R3 from stdin
					reg_file[3] = static_cast<unsigned int>(cin.get());
					break;
				case 98: //Print all registers to stdout
					for(size_t i = 0; i < num_gen_regs; ++i){
						if(i <= 15){
							cout << "R" << i << "\t";
						} else {
							switch(i){
								case 16: cout << "PC\t"; break;
                case 17: cout << "SL\t"; break;
                case 18: cout << "SB\t"; break;
                case 19: cout << "SP\t"; break;
                case 20: cout << "FP\t"; break;
                case 21: cout << "HP\t"; break;
							}
						}
					}
					cout << reg_file[1] << endl;
					break;
				default:
					return false;
			}
		break;
	}	
	return true;
}

