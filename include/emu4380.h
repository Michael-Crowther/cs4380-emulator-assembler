#ifndef EMU_4380_H
#define EMU_4380_H
#include<array>

constexpr size_t num_gen_regs = 22;
constexpr size_t num_cntrl_regs = 5;
constexpr size_t num_data_regs = 2;

enum RegNames{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, PC, SL, SB, SP, FP, HP = 21
};

enum CntrlRegNames{
	OPERATION=0, OPERAND_1, OPERAND_2, OPERAND_3, IMMEDIATE
};

enum DataRegNames{
	REG_VAL_1, REG_VAL_2
};

//Cache base class
class Cache {
  public:
    virtual unsigned char readByte(unsigned int address) = 0;
    virtual unsigned int readWord(unsigned int address) = 0;
    virtual void writeByte(unsigned int address, unsigned char byte) = 0;
    virtual void writeWord(unsigned int address, unsigned int word) = 0;
    virtual void init(unsigned int cacheType) = 0;
    virtual ~Cache() = default;
};

class NoCache : public Cache {
  public:
    unsigned char readByte(unsigned int address) override;
    unsigned int readWord(unsigned int address) override;
    void writeByte(unsigned int address, unsigned char byte) override;
    void writeWord(unsigned int address, unsigned int word) override;
    void init(unsigned int cacheType) override;
};

/*
class DirectMappedCache : public Cache {
  public:
    unsigned char readByte(unsigned int address) override;
    unsigned int readWord(unsigned int address) override;
    void writeByte(unsigned int address, unsigned char byte) override;
    void writeWord(unsigned int address, unsigned int word) override;
    void init(unsigned int cacheType) override;
};

class FullyAssociativeCache : public Cache {
  public:
    unsigned char readByte(unsigned int address) override;
    unsigned int readWord(unsigned int address) override;
    void writeByte(unsigned int address, unsigned char byte) override;
    void writeWord(unsigned int address, unsigned int word) override;
    void init(unsigned int cacheType) override;
};
*/

extern unsigned int reg_file[num_gen_regs];
extern unsigned char* prog_mem;
extern unsigned int cntrl_regs[num_cntrl_regs];
extern unsigned int data_regs[num_data_regs];
extern unsigned int memorySize;
extern unsigned int mem_cycle_cntr;
extern Cache* globalCache;

bool fetch();

bool decode();

bool execute();

bool init_mem(unsigned int size);

bool isValidRegister(unsigned int reg);

void init_cache(unsigned int cacheType);
#endif
