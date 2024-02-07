#ifndef EMU_4380_H
#define EMU_4380_H
#include<array>

constexpr size_t num_gen_regs = 22;
constexpr size_t num_cntrl_regs = 5;
constexpr size_t num_data_regs = 2;

enum class RegNames{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, PC, SL, SB, SP, FP, HP = 21
};

enum class CntrlRegNames{
	OPERATION=0, OPERAND_1, OPERAND_2, OPERAND_3, IMMEDIATE
};

enum class DataRegNames{
	REG_VAL_1, REG_VAL_2
};

extern unsigned int reg_file[num_gen_regs];
extern unsigned char* prog_mem;
extern unsigned int cntrl_regs[num_cntrl_regs];
extern unsigned int data_regs[num_data_regs];
extern unsigned int memorySize;

bool fetch();

bool decode();

bool execute();

bool init_mem(unsigned int size);

#endif
